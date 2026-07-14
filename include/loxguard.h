#ifndef LOXGUARD_H
#define LOXGUARD_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOXGUARD_VERSION_MAJOR 1
#define LOXGUARD_VERSION_MINOR 0
#define LOXGUARD_VERSION_PATCH 2

#define LOXGUARD_BLACKBOX_MAX_EVENTS_DEFAULT 16u
#define LOXGUARD_BLACKBOX_STR_MAX_DEFAULT 64u

#if defined(LOX_BLACKBOX_MAX_EVENTS) && (LOX_BLACKBOX_MAX_EVENTS != LOXGUARD_BLACKBOX_MAX_EVENTS_DEFAULT)
#error "LOX_BLACKBOX_MAX_EVENTS must match the stable blackbox layout"
#endif
#if defined(LOX_BLACKBOX_STR_MAX) && (LOX_BLACKBOX_STR_MAX != LOXGUARD_BLACKBOX_STR_MAX_DEFAULT)
#error "LOX_BLACKBOX_STR_MAX must match the stable blackbox layout"
#endif

typedef enum {
    LOXGUARD_OK = 0,
    LOXGUARD_ERR_BOUNDS = -1,
    LOXGUARD_ERR_NULL = -2,
    LOXGUARD_ERR_READONLY = -3,
    LOXGUARD_ERR_OVERFLOW = -4,
    LOXGUARD_ERR_ALIGN = -5,
    LOXGUARD_ERR_ARENA_FULL = -6,
    LOXGUARD_ERR_TIMEOUT = -7,
    LOXGUARD_ERR_UNSUPPORTED = -8,
    LOXGUARD_ERR_MEMORY_FAULT = -9
} loxguard_status_t;

typedef enum {
    LOX_EVENT_NONE = 0,
    LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS = 1,
    LOX_EVENT_BLOCK_ARENA_OVERFLOW = 2,
    LOX_EVENT_BLOCK_TIMEOUT = 3,
    LOX_EVENT_BLOCK_MEMORY_FAULT = 4,
    LOX_EVENT_BLOCK_UNSUPPORTED = 5,
    LOX_EVENT_BLOCK_ENTERED = 6,
    LOX_EVENT_BLOCK_OK = 7,
    LOX_EVENT_BLOCK_COMPLETED = 8,
    LOX_EVENT_BLOCK_ERROR = 9,
    LOX_EVENT_BLOCK_PANIC = 10,
    LOX_EVENT_BLOCK_FAULT = 11
} lox_event_kind_t;

typedef enum {
    LOX_ACTION_NONE = 0,
    LOX_ACTION_DROP_INPUT = 1,
    LOX_ACTION_RESET_BLOCK = 2
} lox_action_t;

typedef enum {
    LOX_RESULT_NONE = 0,
    LOX_RESULT_OK = 1,
    LOX_RESULT_ERROR = 2,
    LOX_RESULT_TIMEOUT = 3,
    LOX_RESULT_BOUNDS = 4,
    LOX_RESULT_ARENA = 5,
    LOX_RESULT_MEMORY_FAULT = 6,
    LOX_RESULT_UNSUPPORTED = 7
} lox_result_t;

#define LOX_SPAN_FLAG_READ  (1u << 0)
#define LOX_SPAN_FLAG_WRITE (1u << 1)

typedef struct {
    uint8_t *base;
    size_t len;
    uint32_t flags;
} lox_span_t;

typedef struct {
    uint8_t *base;
    size_t size;
    size_t used;
} lox_arena_t;

typedef struct {
    lox_event_kind_t kind;
    /* Borrowed pointers supplied by the emitter; blackbox storage copies them. */
    const char *block_name;
    /* Borrowed pointers supplied by the emitter; blackbox storage copies them. */
    const char *reason;
    size_t index;
    size_t limit;
    uint32_t aux_code;
} lox_event_t;

#ifndef LOX_BLACKBOX_MAX_EVENTS
#define LOX_BLACKBOX_MAX_EVENTS LOXGUARD_BLACKBOX_MAX_EVENTS_DEFAULT
#endif

#ifndef LOX_BLACKBOX_STR_MAX
#define LOX_BLACKBOX_STR_MAX LOXGUARD_BLACKBOX_STR_MAX_DEFAULT
#endif

typedef struct {
    lox_event_t events[LOX_BLACKBOX_MAX_EVENTS];
    char block_names[LOX_BLACKBOX_MAX_EVENTS][LOX_BLACKBOX_STR_MAX];
    char reasons[LOX_BLACKBOX_MAX_EVENTS][LOX_BLACKBOX_STR_MAX];
    size_t count;
} lox_blackbox_t;

typedef struct {
    /* Borrowed pointers owned by the caller or by the active report source. */
    const char *last_block;
    /* Borrowed pointers owned by the caller or by the active report source. */
    const char *last_failed_block;
    /* Borrowed pointers owned by the caller or by the active report source. */
    const char *reason;
    lox_result_t result;
    lox_action_t action;
    uint32_t duration_ticks;
    int event_persisted;
} lox_report_t;

typedef struct {
    lox_span_t input;
    lox_span_t output;
    lox_arena_t *scratch;
    const char *block_name;
    lox_event_t last_event;
    lox_blackbox_t *blackbox;
    uint32_t start_ticks;
    uint32_t duration_ticks;
    int last_event_persisted;
} lox_guard_ctx_t;

/*
 * Notification-only callback hook.
 *
 * The callback observes the final event/action pair for a run. It does not own
 * the event payload, cannot signal an error back into loxguard, and is not a
 * cleanup/defer hook for user resources.
 */
typedef void (*lox_recovery_cb_t)(const lox_event_t *event, lox_action_t action, void *user_ctx);

typedef enum {
    LOXGUARD_OPTIONAL = 0,
    LOXGUARD_CRITICAL = 1
} loxguard_criticality_t;

typedef struct {
    const char *name;
    uint32_t timeout_ms;
    loxguard_criticality_t criticality;
    uint32_t max_failures;
    /* Caller-owned spans and scratch memory; loxguard borrows them for the run. */
    lox_span_t input;
    lox_span_t output;
    void *scratch;
    size_t scratch_len;
    /* Caller-owned blackbox storage; loxguard copies events into it. */
    lox_blackbox_t *blackbox;
} loxguard_block_cfg_t;

typedef int (*loxguard_fn_t)(lox_guard_ctx_t *guard, void *user_ctx);

void *lox_arena_alloc(lox_arena_t *a, size_t size, size_t align);

static inline int loxguard_checked_mul_size(size_t a, size_t b, size_t *out) {
    if (out == NULL) {
        return 0;
    }
    if (a != 0u && b > ((size_t)-1) / a) {
        return 0;
    }
    *out = a * b;
    return 1;
}

static inline void *loxguard_arena_alloc_array(lox_arena_t *a, size_t element_size, size_t count, size_t align) {
    size_t total;

    if (!loxguard_checked_mul_size(element_size, count, &total)) {
        return NULL;
    }
    return lox_arena_alloc(a, total, align);
}

lox_span_t lox_span_readonly(const void *ptr, size_t len);
lox_span_t lox_span_writable(void *ptr, size_t len);

int lox_span_read_u8(const lox_span_t *s, size_t index, uint8_t *out);
int lox_span_write_u8(lox_span_t *s, size_t index, uint8_t value);
int lox_span_read(const lox_span_t *s, size_t offset, void *out, size_t len);
int lox_span_write(lox_span_t *s, size_t offset, const void *src, size_t len);
int lox_span_memcpy(
    lox_span_t *dst, size_t dst_off,
    const lox_span_t *src, size_t src_off,
    size_t len
);

void lox_arena_init(lox_arena_t *a, void *mem, size_t size);
void lox_arena_reset(lox_arena_t *a);
size_t lox_arena_used(const lox_arena_t *a);
size_t lox_arena_remaining(const lox_arena_t *a);

void lox_blackbox_init(lox_blackbox_t *bb);
void lox_blackbox_store(lox_blackbox_t *bb, const lox_event_t *event);
lox_action_t lox_policy_decide(const lox_event_t *event);

void lox_guard_emit_bounds(lox_guard_ctx_t *ctx, size_t index, size_t limit);
int lox_checked_output_write_u8(lox_guard_ctx_t *ctx, size_t index, uint8_t value);

void lox_set_recovery_callback(lox_recovery_cb_t cb, void *user_ctx);

lox_report_t loxguard_run(const loxguard_block_cfg_t *cfg, loxguard_fn_t fn, void *user_ctx);

lox_report_t lox_run_checked_parser_demo(
    const uint8_t *in, size_t in_len,
    uint8_t *out, size_t out_len,
    void *scratch, size_t scratch_len,
    lox_blackbox_t *blackbox
);

lox_report_t lox_run_checked_parser_timeout_demo(lox_blackbox_t *blackbox);
lox_report_t lox_run_guard_panic_demo(lox_blackbox_t *blackbox);
lox_report_t lox_run_guard_fault_demo(lox_blackbox_t *blackbox);
lox_report_t lox_run_rtos_timeout_demo(lox_blackbox_t *blackbox, const char *task_name, uint32_t tick_budget);
lox_report_t lox_run_mpu_fault_demo(lox_blackbox_t *blackbox, const char *block_name, uint32_t fault_addr, uint32_t cfsr, uint32_t hfsr);

#ifdef __cplusplus
}
#endif

#endif
