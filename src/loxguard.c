#include "loxguard.h"
#include "loxguard_adapters.h"
#include "loxguard_backends.h"
#include "loxguard_ports.h"
#ifdef LOXGUARD_HAVE_SAFEMATH
#include "safemath.h"
#endif

#include <string.h>

static lox_recovery_cb_t g_recovery_cb = NULL;
static void *g_recovery_user_ctx = NULL;

#define LOX_FAILURE_STREAK_SLOTS 8u

typedef struct {
    int in_use;
    char block_name[64];
    uint32_t streak;
} lox_failure_streak_t;

static lox_failure_streak_t g_failure_streaks[LOX_FAILURE_STREAK_SLOTS];

static uint32_t *lox_failure_streak_for_block(const char *block_name) {
    size_t i;
    size_t free_idx = LOX_FAILURE_STREAK_SLOTS;
    const char *key = (block_name == NULL || block_name[0] == '\0') ? "global" : block_name;

    for (i = 0u; i < LOX_FAILURE_STREAK_SLOTS; i++) {
        if (g_failure_streaks[i].in_use) {
            if (strncmp(g_failure_streaks[i].block_name, key, sizeof(g_failure_streaks[i].block_name)) == 0) {
                return &g_failure_streaks[i].streak;
            }
        } else if (free_idx == LOX_FAILURE_STREAK_SLOTS) {
            free_idx = i;
        }
    }

    if (free_idx == LOX_FAILURE_STREAK_SLOTS) {
        return &g_failure_streaks[0].streak;
    }

    g_failure_streaks[free_idx].in_use = 1;
    strncpy(g_failure_streaks[free_idx].block_name, key, sizeof(g_failure_streaks[free_idx].block_name) - 1u);
    g_failure_streaks[free_idx].block_name[sizeof(g_failure_streaks[free_idx].block_name) - 1u] = '\0';
    g_failure_streaks[free_idx].streak = 0u;
    return &g_failure_streaks[free_idx].streak;
}

static int lox_safe_add_size(size_t *out, size_t a, size_t b) {
#ifdef LOXGUARD_HAVE_SAFEMATH
    return sm_add(out, a, b) ? 1 : 0;
#else
    if (out == NULL || a > (size_t)-1 - b) {
        return 0;
    }
    *out = a + b;
    return 1;
#endif
}

static int lox_emit_event_ex(
    lox_guard_ctx_t *ctx,
    lox_event_kind_t kind,
    const char *reason,
    size_t index,
    size_t limit,
    uint32_t aux_code,
    int track_as_last
) {
    lox_event_t ev;
    int persist_rc;
    int persisted;
    uint32_t *failure_streak;

    if (ctx == NULL || ctx->blackbox == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    memset(&ev, 0, sizeof(ev));
    failure_streak = lox_failure_streak_for_block(ctx->block_name);
    ev.kind = kind;
    ev.block_name = ctx->block_name;
    ev.reason = reason;
    ev.index = index;
    ev.limit = limit;
    ev.aux_code = aux_code;
    lox_blackbox_store(ctx->blackbox, &ev);

    (void)lox_adapter_log_event(&ev);
    persist_rc = lox_adapter_persist_event(&ev);
    lox_adapter_watchdog_observe_event(&ev);
    persisted = (persist_rc == LOXGUARD_OK) ? 1 : 0;

    if (track_as_last) {
        ctx->last_event = ev;
        ctx->last_event_persisted = persisted;
    }

    if (kind == LOX_EVENT_BLOCK_ENTERED) {
        lox_adapter_health_set_for_block(ctx->block_name, 1);
    } else if (kind == LOX_EVENT_BLOCK_OK) {
        *failure_streak = 0u;
        lox_adapter_health_set_for_block(ctx->block_name, 0);
    } else if (kind == LOX_EVENT_BLOCK_COMPLETED) {
        /* Keep previous state; completed can follow both success and failure incidents. */
    } else if (kind == LOX_EVENT_BLOCK_TIMEOUT) {
        (*failure_streak)++;
        lox_adapter_health_set_for_block(ctx->block_name, 2);
    } else if (kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
               kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
               kind == LOX_EVENT_BLOCK_MEMORY_FAULT ||
               kind == LOX_EVENT_BLOCK_PANIC ||
               kind == LOX_EVENT_BLOCK_FAULT ||
               kind == LOX_EVENT_BLOCK_ERROR) {
        (*failure_streak)++;
        lox_adapter_health_set_for_block(ctx->block_name, 3);
    } else if (kind == LOX_EVENT_BLOCK_UNSUPPORTED) {
        (*failure_streak)++;
        lox_adapter_health_set_for_block(ctx->block_name, 4);
    }

    if (*failure_streak >= 5u) {
        lox_adapter_health_set_for_block(ctx->block_name, 5);
    } else if (*failure_streak >= 3u) {
        lox_adapter_health_set_for_block(ctx->block_name, 4);
    }

    return LOXGUARD_OK;
}

lox_span_t lox_span_readonly(const void *ptr, size_t len) {
    lox_span_t s;
    s.base = (uint8_t *)ptr;
    s.len = len;
    s.flags = LOX_SPAN_FLAG_READ;
    return s;
}

lox_span_t lox_span_writable(void *ptr, size_t len) {
    lox_span_t s;
    s.base = (uint8_t *)ptr;
    s.len = len;
    s.flags = LOX_SPAN_FLAG_READ | LOX_SPAN_FLAG_WRITE;
    return s;
}

int lox_span_read_u8(const lox_span_t *s, size_t index, uint8_t *out) {
    if (s == NULL || out == NULL || s->base == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if ((s->flags & LOX_SPAN_FLAG_READ) == 0u) {
        return LOXGUARD_ERR_READONLY;
    }
    if (index >= s->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    *out = s->base[index];
    return LOXGUARD_OK;
}

int lox_span_write_u8(lox_span_t *s, size_t index, uint8_t value) {
    if (s == NULL || s->base == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if ((s->flags & LOX_SPAN_FLAG_WRITE) == 0u) {
        return LOXGUARD_ERR_READONLY;
    }
    if (index >= s->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    s->base[index] = value;
    return LOXGUARD_OK;
}

int lox_span_read(const lox_span_t *s, size_t offset, void *out, size_t len) {
    size_t end;
    if (s == NULL || out == NULL || s->base == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if ((s->flags & LOX_SPAN_FLAG_READ) == 0u) {
        return LOXGUARD_ERR_READONLY;
    }
    if (offset > s->len || !lox_safe_add_size(&end, offset, len) || end > s->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    if (len > 0u) {
        memcpy(out, s->base + offset, len);
    }
    return LOXGUARD_OK;
}

int lox_span_write(lox_span_t *s, size_t offset, const void *src, size_t len) {
    size_t end;
    if (s == NULL || src == NULL || s->base == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if ((s->flags & LOX_SPAN_FLAG_WRITE) == 0u) {
        return LOXGUARD_ERR_READONLY;
    }
    if (offset > s->len || !lox_safe_add_size(&end, offset, len) || end > s->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    if (len > 0u) {
        memcpy(s->base + offset, src, len);
    }
    return LOXGUARD_OK;
}

int lox_span_memcpy(
    lox_span_t *dst, size_t dst_off,
    const lox_span_t *src, size_t src_off,
    size_t len
) {
    size_t src_end;
    size_t dst_end;
    if (dst == NULL || src == NULL || dst->base == NULL || src->base == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    if ((src->flags & LOX_SPAN_FLAG_READ) == 0u || (dst->flags & LOX_SPAN_FLAG_WRITE) == 0u) {
        return LOXGUARD_ERR_READONLY;
    }
    if (src_off > src->len || !lox_safe_add_size(&src_end, src_off, len) || src_end > src->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    if (dst_off > dst->len || !lox_safe_add_size(&dst_end, dst_off, len) || dst_end > dst->len) {
        return LOXGUARD_ERR_BOUNDS;
    }
    if (len > 0u) {
        memcpy(dst->base + dst_off, src->base + src_off, len);
    }
    return LOXGUARD_OK;
}

void lox_arena_init(lox_arena_t *a, void *mem, size_t size) {
    if (a == NULL) {
        return;
    }
    a->base = (uint8_t *)mem;
    a->size = size;
    a->used = 0u;
}

void *lox_arena_alloc(lox_arena_t *a, size_t size, size_t align) {
    size_t aligned;
    size_t end;
    uintptr_t base_addr;
    uintptr_t cur_addr;
    uintptr_t aligned_addr;

    if (a == NULL || a->base == NULL || align == 0u || (align & (align - 1u)) != 0u) {
        return NULL;
    }

    base_addr = (uintptr_t)a->base;
    if (a->used > a->size || base_addr > (uintptr_t)-1 - a->used) {
        return NULL;
    }
    cur_addr = base_addr + a->used;
    aligned_addr = (cur_addr + (uintptr_t)(align - 1u)) & ~((uintptr_t)align - 1u);
    if (aligned_addr < base_addr) {
        return NULL;
    }
    aligned = (size_t)(aligned_addr - base_addr);
    if (aligned > a->size) {
        return NULL;
    }
    if (!lox_safe_add_size(&end, aligned, size) || end > a->size) {
        return NULL;
    }

    a->used = end;
    return (void *)(a->base + aligned);
}

void lox_arena_reset(lox_arena_t *a) {
    if (a != NULL) {
        a->used = 0u;
    }
}

size_t lox_arena_used(const lox_arena_t *a) {
    return (a == NULL) ? 0u : a->used;
}

size_t lox_arena_remaining(const lox_arena_t *a) {
    if (a == NULL || a->used > a->size) {
        return 0u;
    }
    return a->size - a->used;
}

void lox_blackbox_init(lox_blackbox_t *bb) {
    if (bb == NULL) {
        return;
    }
    bb->count = 0u;
    memset(bb->events, 0, sizeof(bb->events));
    memset(bb->block_names, 0, sizeof(bb->block_names));
    memset(bb->reasons, 0, sizeof(bb->reasons));
}

void lox_blackbox_store(lox_blackbox_t *bb, const lox_event_t *event) {
    size_t idx;
    size_t i;
    const char *block;
    const char *reason;

    if (bb == NULL || event == NULL) {
        return;
    }

    block = (event->block_name == NULL) ? "none" : event->block_name;
    reason = (event->reason == NULL) ? "none" : event->reason;

    if (bb->count < (sizeof(bb->events) / sizeof(bb->events[0]))) {
        idx = bb->count++;
    } else {
        memmove(&bb->events[0], &bb->events[1], (bb->count - 1u) * sizeof(bb->events[0]));
        memmove(&bb->block_names[0], &bb->block_names[1], (bb->count - 1u) * sizeof(bb->block_names[0]));
        memmove(&bb->reasons[0], &bb->reasons[1], (bb->count - 1u) * sizeof(bb->reasons[0]));
        idx = bb->count - 1u;
    }

    bb->events[idx] = *event;
    strncpy(bb->block_names[idx], block, sizeof(bb->block_names[idx]) - 1u);
    bb->block_names[idx][sizeof(bb->block_names[idx]) - 1u] = '\0';
    strncpy(bb->reasons[idx], reason, sizeof(bb->reasons[idx]) - 1u);
    bb->reasons[idx][sizeof(bb->reasons[idx]) - 1u] = '\0';
    for (i = 0u; i < bb->count; i++) {
        bb->events[i].block_name = bb->block_names[i];
        bb->events[i].reason = bb->reasons[i];
    }
}

lox_action_t lox_policy_decide(const lox_event_t *event) {
    if (event == NULL) {
        return LOX_ACTION_NONE;
    }
    if (event->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
        event->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
        event->kind == LOX_EVENT_BLOCK_TIMEOUT ||
        event->kind == LOX_EVENT_BLOCK_MEMORY_FAULT ||
        event->kind == LOX_EVENT_BLOCK_PANIC ||
        event->kind == LOX_EVENT_BLOCK_FAULT ||
        event->kind == LOX_EVENT_BLOCK_UNSUPPORTED ||
        event->kind == LOX_EVENT_BLOCK_ERROR) {
        return LOX_ACTION_DROP_INPUT;
    }
    return LOX_ACTION_RESET_BLOCK;
}

void lox_guard_emit_bounds(lox_guard_ctx_t *ctx, size_t index, size_t limit) {
    if (ctx == NULL) {
        return;
    }
    (void)lox_emit_event_ex(ctx, LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "BOUNDS", index, limit, 0u, 1);
}

int lox_checked_output_write_u8(lox_guard_ctx_t *ctx, size_t index, uint8_t value) {
    int rc;

    if (ctx == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    rc = lox_span_write_u8(&ctx->output, index, value);
    if (rc == LOXGUARD_ERR_BOUNDS) {
        lox_guard_emit_bounds(ctx, index, ctx->output.len);
    }
    return rc;
}

void lox_set_recovery_callback(lox_recovery_cb_t cb, void *user_ctx) {
    g_recovery_cb = cb;
    g_recovery_user_ctx = user_ctx;
}

static void lox_invoke_recovery(const lox_event_t *event, lox_action_t action) {
    if (g_recovery_cb != NULL) {
        g_recovery_cb(event, action, g_recovery_user_ctx);
    }
}

static int packet_parser_block(lox_guard_ctx_t *ctx) {
    uint8_t temp;

    if (ctx == NULL || ctx->scratch == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    if (lox_arena_alloc(ctx->scratch, 8u, 4u) == NULL) {
        ctx->last_event.kind = LOX_EVENT_BLOCK_ARENA_OVERFLOW;
        ctx->last_event.block_name = ctx->block_name;
        ctx->last_event.reason = "ARENA";
        ctx->last_event.index = lox_arena_used(ctx->scratch);
        ctx->last_event.limit = ctx->scratch->size;
        ctx->last_event.aux_code = 0u;
        (void)lox_emit_event_ex(
            ctx,
            LOX_EVENT_BLOCK_ARENA_OVERFLOW,
            "ARENA",
            lox_arena_used(ctx->scratch),
            ctx->scratch->size,
            0u,
            1
        );
        return LOXGUARD_ERR_ARENA_FULL;
    }

    if (ctx->input.len > 0u) {
        (void)lox_span_read_u8(&ctx->input, 0u, &temp);
    }

    return lox_checked_output_write_u8(ctx, 17u, 0xABu);
}

static lox_report_t lox_finalize_report(const lox_guard_ctx_t *ctx, int rc) {
    lox_report_t report;
    report.last_block = ctx->block_name;
    report.last_failed_block = NULL;
    report.reason = "NONE";
    report.result = LOX_RESULT_NONE;
    report.action = LOX_ACTION_NONE;
    report.duration_ticks = ctx->duration_ticks;
    report.event_persisted = ctx->last_event_persisted;

    if (rc != LOXGUARD_OK) {
        report.last_failed_block = ctx->block_name;
        report.reason = (ctx->last_event.reason == NULL) ? "ERROR" : ctx->last_event.reason;
        report.action = lox_policy_decide(&ctx->last_event);
        if (ctx->last_event.kind == LOX_EVENT_BLOCK_TIMEOUT) {
            report.result = LOX_RESULT_TIMEOUT;
        } else if (ctx->last_event.kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS) {
            report.result = LOX_RESULT_BOUNDS;
        } else if (ctx->last_event.kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW) {
            report.result = LOX_RESULT_ARENA;
        } else if (ctx->last_event.kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
            report.result = LOX_RESULT_MEMORY_FAULT;
        } else if (ctx->last_event.kind == LOX_EVENT_BLOCK_UNSUPPORTED) {
            report.result = LOX_RESULT_UNSUPPORTED;
        } else {
            report.result = LOX_RESULT_ERROR;
        }
        lox_invoke_recovery(&ctx->last_event, report.action);
    } else {
        report.result = LOX_RESULT_OK;
    }

    return report;
}

static int lox_emit_unsupported(lox_guard_ctx_t *ctx, const char *reason) {
    if (ctx == NULL || ctx->blackbox == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    ctx->last_event.kind = LOX_EVENT_BLOCK_UNSUPPORTED;
    ctx->last_event.block_name = ctx->block_name;
    ctx->last_event.reason = reason;
    ctx->last_event.index = 0u;
    ctx->last_event.limit = 0u;
    ctx->last_event.aux_code = 0u;
    (void)lox_emit_event_ex(ctx, LOX_EVENT_BLOCK_UNSUPPORTED, reason, 0u, 0u, 0u, 1);
    return LOXGUARD_ERR_UNSUPPORTED;
}

lox_report_t lox_run_checked_parser_demo(
    const uint8_t *in, size_t in_len,
    uint8_t *out, size_t out_len,
    void *scratch, size_t scratch_len,
    lox_blackbox_t *blackbox
) {
    lox_guard_ctx_t ctx;
    lox_arena_t arena;
    int rc;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    lox_arena_init(&arena, scratch, scratch_len);

    ctx.input = lox_span_readonly(in, in_len);
    ctx.output = lox_span_writable(out, out_len);
    ctx.scratch = &arena;
    ctx.block_name = "packet_parser";
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = 0u;
    ctx.last_event_persisted = 0;
    memset(&ctx.last_event, 0, sizeof(ctx.last_event));
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    if (!lox_adapter_recovery_allow_attempt_for_block(ctx.block_name)) {
        (void)lox_emit_event_ex(
            &ctx,
            LOX_EVENT_BLOCK_ERROR,
            "BREAKER_OPEN",
            0u,
            0u,
            (uint32_t)lox_adapter_recovery_state_get(),
            1
        );
        end_ticks = lox_adapter_now_ms();
        ctx.duration_ticks = end_ticks - ctx.start_ticks;
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
        return lox_finalize_report(&ctx, LOXGUARD_ERR_UNSUPPORTED);
    }

    rc = packet_parser_block(&ctx);
    if (rc == LOXGUARD_OK) {
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_OK, "OK", 0u, 0u, 0u, 1);
    } else if (ctx.last_event.kind == LOX_EVENT_NONE) {
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ERROR, "ERROR", 0u, 0u, 0u, 1);
    }
    lox_adapter_recovery_report_result_for_block(ctx.block_name, rc == LOXGUARD_OK);
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
    return lox_finalize_report(&ctx, rc);
}

lox_report_t lox_run_checked_parser_timeout_demo(lox_blackbox_t *blackbox) {
    lox_guard_ctx_t ctx;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    memset(&ctx, 0, sizeof(ctx));
    ctx.block_name = "packet_parser";
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_TIMEOUT, "TIMEOUT", 0u, 0u, 0u, 1);
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);

    return lox_finalize_report(&ctx, LOXGUARD_ERR_TIMEOUT);
}

lox_report_t lox_run_guard_panic_demo(lox_blackbox_t *blackbox) {
    lox_guard_ctx_t ctx;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    memset(&ctx, 0, sizeof(ctx));
    ctx.block_name = "panic_guard_block";
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    lox_adapter_panic_hook("PANIC");
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_PANIC, "PANIC", 0u, 0u, 1u, 1);
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
    return lox_finalize_report(&ctx, LOXGUARD_ERR_UNSUPPORTED);
}

lox_report_t lox_run_guard_fault_demo(lox_blackbox_t *blackbox) {
    lox_guard_ctx_t ctx;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    memset(&ctx, 0, sizeof(ctx));
    ctx.block_name = "fault_guard_block";
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_FAULT, "FAULT", 0u, 0u, 2u, 1);
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
    return lox_finalize_report(&ctx, LOXGUARD_ERR_UNSUPPORTED);
}

lox_report_t lox_run_rtos_timeout_demo(lox_blackbox_t *blackbox, const char *task_name, uint32_t tick_budget) {
    lox_guard_ctx_t ctx;
    int rc;
    lox_port_info_t port;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    memset(&ctx, 0, sizeof(ctx));
    ctx.block_name = (task_name == NULL) ? "rtos_task" : task_name;
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    port = lox_port_get_active();

    if (!lox_port_supports_rtos(port.id)) {
        rc = lox_emit_unsupported(&ctx, "RTOS_UNSUPPORTED");
        end_ticks = lox_adapter_now_ms();
        ctx.duration_ticks = end_ticks - ctx.start_ticks;
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
        return lox_finalize_report(&ctx, rc);
    }

    rc = lox_rtos_report_timeout(blackbox, ctx.block_name, tick_budget);
    if (rc == LOXGUARD_OK && blackbox->count > 0u) {
        int persist_rc;
        ctx.last_event = blackbox->events[blackbox->count - 1u];
        (void)lox_adapter_log_event(&ctx.last_event);
        persist_rc = lox_adapter_persist_event(&ctx.last_event);
        ctx.last_event_persisted = (persist_rc == LOXGUARD_OK) ? 1 : 0;
        rc = LOXGUARD_ERR_TIMEOUT;
    } else if (rc != LOXGUARD_OK && ctx.last_event.kind == LOX_EVENT_NONE) {
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ERROR, "ERROR", 0u, 0u, 0u, 1);
    }
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
    return lox_finalize_report(&ctx, rc);
}

lox_report_t lox_run_mpu_fault_demo(lox_blackbox_t *blackbox, const char *block_name, uint32_t fault_addr, uint32_t cfsr, uint32_t hfsr) {
    lox_guard_ctx_t ctx;
    lox_mpu_fault_ctx_t fault;
    int rc;
    lox_port_info_t port;
    uint32_t end_ticks;

    lox_blackbox_init(blackbox);
    memset(&ctx, 0, sizeof(ctx));
    ctx.block_name = (block_name == NULL) ? "mpu_block" : block_name;
    ctx.blackbox = blackbox;
    ctx.start_ticks = lox_adapter_now_ms();
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ENTERED, "ENTERED", 0u, 0u, 0u, 0);
    port = lox_port_get_active();

    if (!lox_port_supports_mpu(port.id)) {
        rc = lox_emit_unsupported(&ctx, "MPU_UNSUPPORTED");
        end_ticks = lox_adapter_now_ms();
        ctx.duration_ticks = end_ticks - ctx.start_ticks;
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
        return lox_finalize_report(&ctx, rc);
    }

    fault.fault_addr = fault_addr;
    fault.cfsr = cfsr;
    fault.hfsr = hfsr;

    rc = lox_mpu_report_fault(blackbox, ctx.block_name, &fault);
    if (rc == LOXGUARD_OK && blackbox->count > 0u) {
        int persist_rc;
        ctx.last_event = blackbox->events[blackbox->count - 1u];
        (void)lox_adapter_log_event(&ctx.last_event);
        persist_rc = lox_adapter_persist_event(&ctx.last_event);
        ctx.last_event_persisted = (persist_rc == LOXGUARD_OK) ? 1 : 0;
        rc = LOXGUARD_ERR_TIMEOUT;
    } else if (rc != LOXGUARD_OK && ctx.last_event.kind == LOX_EVENT_NONE) {
        (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_ERROR, "ERROR", 0u, 0u, 0u, 1);
    }
    end_ticks = lox_adapter_now_ms();
    ctx.duration_ticks = end_ticks - ctx.start_ticks;
    (void)lox_emit_event_ex(&ctx, LOX_EVENT_BLOCK_COMPLETED, "COMPLETED", ctx.duration_ticks, 0u, 0u, 0);
    return lox_finalize_report(&ctx, rc);
}
