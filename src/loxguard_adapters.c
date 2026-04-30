#include "loxguard_adapters.h"

#ifdef LOXGUARD_HAVE_MICROLOG
#include "mlog.h"
#endif
#ifdef LOXGUARD_HAVE_MICROHEALTH
#include "mhealth.h"
#endif

static lox_time_now_fn_t g_time_now_fn = NULL;
static int g_health_code = 0;

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
    if (g_time_now_fn != NULL) {
        return g_time_now_fn();
    }
    return 0u;
}

void lox_adapter_set_time_now(lox_time_now_fn_t fn) {
    g_time_now_fn = fn;
}

void lox_adapter_watchdog_kick(void) {
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
