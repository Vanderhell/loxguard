#include "loxguard_adapters.h"

#ifdef LOXGUARD_HAVE_MICROLOG
#include "mlog.h"
#endif
#ifdef LOXGUARD_HAVE_MICROHEALTH
#include "mhealth.h"
#endif
#if defined(LOXGUARD_HAVE_MICROTIMER) && defined(LOXGUARD_USE_MICROTIMER)
#include "mtimer.h"
#endif
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
#include "mwdt.h"
#endif

static lox_time_now_fn_t g_time_now_fn = NULL;
static int g_health_code = 0;
static int g_watchdog_state = 0; /* 0=OK, 1=LATE, 2=STARVED */

#ifdef LOXGUARD_HAVE_MICROHEALTH
static mhealth_t g_mhealth;
static int g_mhealth_ready = 0;
static int32_t g_health_score = 100;

static int32_t lox_collect_health_score(void *ctx) {
    (void)ctx;
    return g_health_score;
}

static uint32_t lox_mhealth_clock(void) {
    return lox_adapter_now_ms();
}

static void lox_mhealth_ensure_init(void) {
    if (g_mhealth_ready) {
        return;
    }
    if (mhealth_init(&g_mhealth, lox_mhealth_clock, 0u) != MHEALTH_OK) {
        return;
    }
    if (mhealth_register(
        &g_mhealth,
        "loxguard_guard_state",
        MHEALTH_METRIC_CUSTOM_BASE,
        lox_collect_health_score,
        NULL,
        MHEALTH_BELOW,
        50,
        10
    ) < 0) {
        return;
    }
    g_mhealth_ready = 1;
}
#endif

#if defined(LOXGUARD_HAVE_MICROTIMER) && defined(LOXGUARD_USE_MICROTIMER)
static mtimer_t g_mtimer;
static int g_mtimer_ready = 0;
static uint8_t g_mtimer_hb_id = 0u;
static int g_mtimer_hb_valid = 0;

static uint32_t lox_mtimer_clock(void) {
    if (g_time_now_fn != NULL) {
        return g_time_now_fn();
    }
    return 0u;
}

static void lox_mtimer_hb_cb(uint8_t timer_id, void *ctx) {
    (void)timer_id;
    (void)ctx;
}

static void lox_mtimer_ensure_init(void) {
    int id;
    if (g_mtimer_ready) {
        return;
    }
    if (mtimer_init(&g_mtimer, lox_mtimer_clock) != MTIMER_OK) {
        return;
    }
    id = mtimer_create(&g_mtimer, "loxguard_hb", 1000u, MTIMER_PERIODIC, lox_mtimer_hb_cb, NULL);
    if (id >= 0) {
        g_mtimer_hb_id = (uint8_t)id;
        g_mtimer_hb_valid = 1;
        (void)mtimer_start(&g_mtimer, g_mtimer_hb_id);
    }
    g_mtimer_ready = 1;
}
#endif

#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
static mwdt_t g_mwdt;
static int g_mwdt_ready = 0;
static int g_mwdt_idx = -1;

static uint32_t lox_mwdt_clock(void) {
    return lox_adapter_now_ms();
}

static void lox_mwdt_timeout_cb(const mwdt_timeout_t *event, void *ctx) {
    (void)ctx;
    if (event == NULL) {
        return;
    }
    if (event->state == MWDT_TASK_STARVED) {
        g_watchdog_state = 2;
    } else if (event->state == MWDT_TASK_LATE && g_watchdog_state < 2) {
        g_watchdog_state = 1;
    }
}

static void lox_mwdt_ensure_init(void) {
    if (g_mwdt_ready) {
        return;
    }
    if (mwdt_init(&g_mwdt, lox_mwdt_clock) != MWDT_OK) {
        return;
    }
    mwdt_set_timeout_cb(&g_mwdt, lox_mwdt_timeout_cb, NULL);
    g_mwdt_idx = mwdt_register(&g_mwdt, "loxguard_guard_block", 5u, 2u, false);
    if (g_mwdt_idx < 0) {
        return;
    }
    g_mwdt_ready = 1;
}
#endif

int lox_adapter_log_event(const lox_event_t *event) {
#ifdef LOXGUARD_HAVE_MICROLOG
    if (event != NULL) {
        mlog_log(
            NULL,
            MLOG_INFO,
            "LOXGUARD",
            "kind=%d block=%s reason=%s index=%zu limit=%zu aux=%u",
            (int)event->kind,
            event->block_name ? event->block_name : "none",
            event->reason ? event->reason : "none",
            event->index,
            event->limit,
            event->aux_code
        );
    }
#else
    (void)event;
#endif
    return LOXGUARD_OK;
}

uint32_t lox_adapter_now_ms(void) {
#if defined(LOXGUARD_HAVE_MICROTIMER) && defined(LOXGUARD_USE_MICROTIMER)
    lox_mtimer_ensure_init();
    if (g_mtimer_ready) {
        (void)mtimer_tick(&g_mtimer);
    }
#endif
    if (g_time_now_fn != NULL) {
        return g_time_now_fn();
    }
    return 0u;
}

void lox_adapter_set_time_now(lox_time_now_fn_t fn) {
    g_time_now_fn = fn;
}

void lox_adapter_watchdog_kick(void) {
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
    lox_mwdt_ensure_init();
    if (g_mwdt_ready) {
        (void)mwdt_kick(&g_mwdt, (uint8_t)g_mwdt_idx);
        g_watchdog_state = 0;
    }
#endif
}

void lox_adapter_watchdog_observe_event(const lox_event_t *event) {
    if (event == NULL) {
        return;
    }
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
    lox_mwdt_ensure_init();
    if (g_mwdt_ready) {
        if (event->kind == LOX_EVENT_BLOCK_ENTERED || event->kind == LOX_EVENT_BLOCK_OK) {
            (void)mwdt_kick(&g_mwdt, (uint8_t)g_mwdt_idx);
            g_watchdog_state = 0;
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_COMPLETED) {
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_TIMEOUT) {
            g_watchdog_state = 1;
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_ERROR ||
            event->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
            event->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
            event->kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
            g_watchdog_state = 2;
            return;
        }
    }
#endif
    if (event->kind == LOX_EVENT_BLOCK_TIMEOUT) {
        g_watchdog_state = 1;
    } else if (event->kind == LOX_EVENT_BLOCK_ERROR ||
               event->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
               event->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
               event->kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
        g_watchdog_state = 2;
    } else if (event->kind == LOX_EVENT_BLOCK_ENTERED ||
               event->kind == LOX_EVENT_BLOCK_OK) {
        g_watchdog_state = 0;
    }
}

int lox_adapter_watchdog_state_get(void) {
    return g_watchdog_state;
}

void lox_adapter_watchdog_reset(void) {
    g_watchdog_state = 0;
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
    if (g_mwdt_ready && g_mwdt_idx >= 0) {
        (void)mwdt_kick(&g_mwdt, (uint8_t)g_mwdt_idx);
    }
#endif
}

void lox_adapter_panic_hook(const char *message) {
    (void)message;
}

void lox_adapter_health_set(int degraded) {
    g_health_code = degraded;
#ifdef LOXGUARD_HAVE_MICROHEALTH
    lox_mhealth_ensure_init();
    switch (degraded) {
        case 0: g_health_score = 100; break; /* OK */
        case 1: g_health_score = 90; break;  /* ACTIVE/RUNNING */
        case 2: g_health_score = 40; break;  /* LATE/STARVED */
        case 3: g_health_score = 5; break;   /* FAULTED */
        case 4: g_health_score = 20; break;  /* DEGRADED */
        case 5: g_health_score = 0; break;   /* DISABLED */
        default: g_health_score = 20; break;
    }
    if (g_mhealth_ready) {
        (void)mhealth_check_now(&g_mhealth);
    }
#endif
}

int lox_adapter_health_get(void) {
    return g_health_code;
}
