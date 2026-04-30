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
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
#include "mres.h"
#endif

#include <string.h>

static lox_time_now_fn_t g_time_now_fn = NULL;
static int g_health_code = 0;
static int g_watchdog_state = 0; /* aggregate state: max over blocks */
static int g_recovery_state = 0; /* aggregate state: max over blocks */

#define LOX_ADAPTER_BLOCK_SLOTS 8u
#define LOX_ADAPTER_BLOCK_NAME_MAX 63u
#define LOX_ADAPTER_DEFAULT_BLOCK "global"

typedef struct {
    int in_use;
    char block_name[64];
    int health_code;
    int watchdog_state; /* 0=OK, 1=LATE, 2=STARVED */
    int recovery_state; /* 0=CLOSED, 1=OPEN, 2=HALF_OPEN */
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    mres_breaker_t breaker;
    int breaker_ready;
#endif
} lox_adapter_block_state_t;

static lox_adapter_block_state_t g_block_states[LOX_ADAPTER_BLOCK_SLOTS];

static const char *lox_block_key(const char *block_name) {
    if (block_name == NULL || block_name[0] == '\0') {
        return LOX_ADAPTER_DEFAULT_BLOCK;
    }
    return block_name;
}

static lox_adapter_block_state_t *lox_block_state_get(const char *block_name) {
    const char *key = lox_block_key(block_name);
    size_t i;
    size_t free_idx = LOX_ADAPTER_BLOCK_SLOTS;

    for (i = 0u; i < LOX_ADAPTER_BLOCK_SLOTS; i++) {
        if (g_block_states[i].in_use) {
            if (strncmp(g_block_states[i].block_name, key, sizeof(g_block_states[i].block_name)) == 0) {
                return &g_block_states[i];
            }
        } else if (free_idx == LOX_ADAPTER_BLOCK_SLOTS) {
            free_idx = i;
        }
    }

    if (free_idx == LOX_ADAPTER_BLOCK_SLOTS) {
        return &g_block_states[0];
    }

    g_block_states[free_idx].in_use = 1;
    strncpy(g_block_states[free_idx].block_name, key, LOX_ADAPTER_BLOCK_NAME_MAX);
    g_block_states[free_idx].block_name[LOX_ADAPTER_BLOCK_NAME_MAX] = '\0';
    g_block_states[free_idx].health_code = 0;
    g_block_states[free_idx].watchdog_state = 0;
    g_block_states[free_idx].recovery_state = 0;
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    g_block_states[free_idx].breaker_ready = 0;
#endif
    return &g_block_states[free_idx];
}

static void lox_refresh_aggregate_states(void) {
    size_t i;
    int max_health = 0;
    int max_watchdog = 0;
    int max_recovery = 0;
    for (i = 0u; i < LOX_ADAPTER_BLOCK_SLOTS; i++) {
        if (!g_block_states[i].in_use) {
            continue;
        }
        if (g_block_states[i].health_code > max_health) {
            max_health = g_block_states[i].health_code;
        }
        if (g_block_states[i].watchdog_state > max_watchdog) {
            max_watchdog = g_block_states[i].watchdog_state;
        }
        if (g_block_states[i].recovery_state > max_recovery) {
            max_recovery = g_block_states[i].recovery_state;
        }
    }
    g_health_code = max_health;
    g_watchdog_state = max_watchdog;
    g_recovery_state = max_recovery;
}

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

#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
static const mres_breaker_policy_t g_mres_policy = {
    3u,    /* failure_threshold */
    1000u, /* recovery_timeout_ms */
    1u     /* half_open_max_calls */
};

static uint32_t lox_mres_clock(void) {
    return lox_adapter_now_ms();
}

static void lox_mres_ensure_init(lox_adapter_block_state_t *slot) {
    if (slot == NULL || slot->breaker_ready) {
        return;
    }
    if (mres_breaker_init(&slot->breaker, &g_mres_policy) != MRES_OK) {
        return;
    }
    slot->breaker_ready = 1;
}

static void lox_sync_recovery_state(lox_adapter_block_state_t *slot) {
    mres_breaker_state_t st;
    if (slot == NULL || !slot->breaker_ready) {
        return;
    }
    st = mres_breaker_state(&slot->breaker);
    if (st == MRES_BREAKER_OPEN) {
        slot->recovery_state = 1;
    } else if (st == MRES_BREAKER_HALF_OPEN) {
        slot->recovery_state = 2;
    } else {
        slot->recovery_state = 0;
    }
    lox_refresh_aggregate_states();
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
    lox_adapter_block_state_t *slot;
    if (event == NULL) {
        return;
    }
    slot = lox_block_state_get(event->block_name);
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
    lox_mwdt_ensure_init();
    if (g_mwdt_ready) {
        if (event->kind == LOX_EVENT_BLOCK_ENTERED || event->kind == LOX_EVENT_BLOCK_OK) {
            (void)mwdt_kick(&g_mwdt, (uint8_t)g_mwdt_idx);
            slot->watchdog_state = 0;
            lox_refresh_aggregate_states();
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_COMPLETED) {
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_TIMEOUT) {
            slot->watchdog_state = 1;
            lox_refresh_aggregate_states();
            return;
        }
        if (event->kind == LOX_EVENT_BLOCK_ERROR ||
            event->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
            event->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
            event->kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
            slot->watchdog_state = 2;
            lox_refresh_aggregate_states();
            return;
        }
    }
#endif
    if (event->kind == LOX_EVENT_BLOCK_TIMEOUT) {
        slot->watchdog_state = 1;
    } else if (event->kind == LOX_EVENT_BLOCK_ERROR ||
               event->kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS ||
               event->kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW ||
               event->kind == LOX_EVENT_BLOCK_MEMORY_FAULT) {
        slot->watchdog_state = 2;
    } else if (event->kind == LOX_EVENT_BLOCK_ENTERED ||
               event->kind == LOX_EVENT_BLOCK_OK) {
        slot->watchdog_state = 0;
    }
    lox_refresh_aggregate_states();
}

int lox_adapter_watchdog_state_get(void) {
    lox_refresh_aggregate_states();
    return g_watchdog_state;
}

int lox_adapter_watchdog_state_get_for_block(const char *block_name) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
    return slot->watchdog_state;
}

void lox_adapter_watchdog_reset(void) {
    size_t i;
    for (i = 0u; i < LOX_ADAPTER_BLOCK_SLOTS; i++) {
        if (g_block_states[i].in_use) {
            g_block_states[i].watchdog_state = 0;
        }
    }
    lox_refresh_aggregate_states();
#if defined(LOXGUARD_HAVE_MICROWDT) && defined(LOXGUARD_USE_MICROWDT)
    if (g_mwdt_ready && g_mwdt_idx >= 0) {
        (void)mwdt_kick(&g_mwdt, (uint8_t)g_mwdt_idx);
    }
#endif
}

int lox_adapter_recovery_allow_attempt(void) {
    return lox_adapter_recovery_allow_attempt_for_block(NULL);
}

int lox_adapter_recovery_allow_attempt_for_block(const char *block_name) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    lox_mres_ensure_init(slot);
    if (slot->breaker_ready) {
        if (mres_breaker_state(&slot->breaker) == MRES_BREAKER_OPEN &&
            mres_breaker_remaining_ms(&slot->breaker, lox_mres_clock) > 0u) {
            lox_sync_recovery_state(slot);
            return 0;
        }
        lox_sync_recovery_state(slot);
    }
#endif
    return 1;
}

void lox_adapter_recovery_report_result(int success) {
    lox_adapter_recovery_report_result_for_block(NULL, success);
}

void lox_adapter_recovery_report_result_for_block(const char *block_name, int success) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    lox_mres_ensure_init(slot);
    if (slot->breaker_ready) {
        if (success) {
            (void)mres_breaker_report_success(&slot->breaker);
        } else {
            (void)mres_breaker_report_failure(&slot->breaker, lox_mres_clock);
        }
        lox_sync_recovery_state(slot);
    }
#else
    (void)block_name;
    (void)success;
#endif
}

int lox_adapter_recovery_state_get(void) {
    lox_refresh_aggregate_states();
    return g_recovery_state;
}

int lox_adapter_recovery_state_get_for_block(const char *block_name) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
    return slot->recovery_state;
}

void lox_adapter_recovery_reset(void) {
    size_t i;
    for (i = 0u; i < LOX_ADAPTER_BLOCK_SLOTS; i++) {
        if (g_block_states[i].in_use) {
            g_block_states[i].recovery_state = 0;
        }
    }
    lox_refresh_aggregate_states();
#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    for (i = 0u; i < LOX_ADAPTER_BLOCK_SLOTS; i++) {
        if (g_block_states[i].in_use) {
            lox_mres_ensure_init(&g_block_states[i]);
            if (g_block_states[i].breaker_ready) {
                (void)mres_breaker_reset(&g_block_states[i].breaker);
            }
        }
    }
#endif
}

void lox_adapter_panic_hook(const char *message) {
    (void)message;
}

void lox_adapter_health_set(int degraded) {
    lox_adapter_health_set_for_block(NULL, degraded);
}

void lox_adapter_health_set_for_block(const char *block_name, int degraded) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
    slot->health_code = degraded;
    lox_refresh_aggregate_states();
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
    lox_refresh_aggregate_states();
    return g_health_code;
}

int lox_adapter_health_get_for_block(const char *block_name) {
    lox_adapter_block_state_t *slot = lox_block_state_get(block_name);
    return slot->health_code;
}
