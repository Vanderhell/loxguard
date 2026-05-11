#include "loxguard.h"
#include "loxguard_adapters.h"
#include "test_common.h"

#include <string.h>

typedef struct {
    int called;
    int fail_rc;
    int emit_bounds;
} run_probe_t;

typedef struct {
    int calls;
    lox_event_kind_t last_kind;
    lox_action_t last_action;
} recovery_probe_t;

static uint32_t g_now = 0u;
static uint32_t now_ms_inc_0(void) { return g_now; }
static uint32_t now_ms_inc_50(void) { g_now += 50u; return g_now; }
static uint32_t now_ms_script[8];
static size_t now_ms_script_n = 0u;
static size_t now_ms_script_i = 0u;

static void now_ms_set_script(const uint32_t *vals, size_t n) {
    size_t i;
    now_ms_script_n = (n > (sizeof(now_ms_script) / sizeof(now_ms_script[0]))) ? (sizeof(now_ms_script) / sizeof(now_ms_script[0])) : n;
    now_ms_script_i = 0u;
    for (i = 0u; i < now_ms_script_n; i++) {
        now_ms_script[i] = vals[i];
    }
}

static uint32_t now_ms_from_script(void) {
    if (now_ms_script_i < now_ms_script_n) {
        return now_ms_script[now_ms_script_i++];
    }
    /* default to last value if over-consumed */
    return (now_ms_script_n == 0u) ? 0u : now_ms_script[now_ms_script_n - 1u];
}

static int probe_block(lox_guard_ctx_t *g, void *user_ctx) {
    run_probe_t *p = (run_probe_t *)user_ctx;
    if (p != NULL) {
        p->called++;
        if (p->emit_bounds) {
            lox_guard_emit_bounds(g, 9u, (g == NULL) ? 0u : g->output.len);
        }
        return p->fail_rc;
    }
    return LOXGUARD_OK;
}

static void recovery_probe_cb(const lox_event_t *event, lox_action_t action, void *user_ctx) {
    recovery_probe_t *p = (recovery_probe_t *)user_ctx;
    if (p == NULL) {
        return;
    }
    p->calls++;
    p->last_action = action;
    p->last_kind = (event == NULL) ? LOX_EVENT_NONE : event->kind;
}

static int scratch_block(lox_guard_ctx_t *g, void *user_ctx) {
    (void)user_ctx;
    if (g == NULL || g->scratch == NULL) {
        return LOXGUARD_ERR_NULL;
    }
    /* Verify arena is usable and bounded. */
    if (lox_arena_remaining(g->scratch) == 0u) {
        return LOXGUARD_ERR_ARENA_FULL;
    }
    if (lox_arena_alloc(g->scratch, 8u, 4u) == NULL) {
        return LOXGUARD_ERR_ARENA_FULL;
    }
    return LOXGUARD_OK;
}

int test_loxguard_run_suite(void) {
    int failed = 0;

    {
        lox_report_t r = loxguard_run(NULL, probe_block, NULL);
        failed |= expect(r.result == LOX_RESULT_ERROR, "loxguard_run NULL cfg -> error report");
        failed |= expect(r.reason != NULL && strcmp(r.reason, "NULL") == 0, "loxguard_run NULL cfg reason");
    }

    {
        loxguard_block_cfg_t cfg;
        memset(&cfg, 0, sizeof(cfg));
        failed |= expect(loxguard_run(&cfg, probe_block, NULL).result == LOX_RESULT_ERROR, "loxguard_run NULL blackbox -> error report");
        failed |= expect(loxguard_run(&cfg, NULL, NULL).result == LOX_RESULT_ERROR, "loxguard_run NULL fn -> error report");
    }

    {
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        lox_report_t r;

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_OK;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "ok_block";
        cfg.timeout_ms = 0u;
        cfg.criticality = LOXGUARD_OPTIONAL;
        cfg.max_failures = 0u;
        cfg.blackbox = &bb;

        r = loxguard_run(&cfg, probe_block, &probe);
        failed |= expect(probe.called == 1, "loxguard_run calls fn");
        failed |= expect(r.result == LOX_RESULT_OK, "loxguard_run OK -> report OK");
        failed |= expect(r.duration_ticks == 0u, "loxguard_run duration is zero with fixed clock");
        failed |= expect(bb.count >= 3u, "loxguard_run emits lifecycle events");
        failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "loxguard_run first event ENTERED");
        failed |= expect(bb.events[bb.count - 2u].kind == LOX_EVENT_BLOCK_OK, "loxguard_run emits OK");
        failed |= expect(bb.events[bb.count - 1u].kind == LOX_EVENT_BLOCK_COMPLETED, "loxguard_run emits COMPLETED");
    }

    {
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        lox_report_t r;

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_ERR_BOUNDS;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "err_noemit";
        cfg.blackbox = &bb;

        r = loxguard_run(&cfg, probe_block, &probe);
        failed |= expect(r.result == LOX_RESULT_BOUNDS || r.result == LOX_RESULT_ERROR, "loxguard_run error -> non-OK result");
        failed |= expect(bb.count >= 3u, "loxguard_run error emits lifecycle events");
        failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "loxguard_run error first event ENTERED");
        failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_ERROR, "loxguard_run error emits BLOCK_ERROR when user emits none");
        failed |= expect(bb.events[bb.count - 1u].kind == LOX_EVENT_BLOCK_COMPLETED, "loxguard_run error emits COMPLETED");
    }

    {
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        lox_report_t r;
        uint8_t out[4] = {0};

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_ERR_BOUNDS;
        probe.emit_bounds = 1;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "err_emit";
        cfg.blackbox = &bb;
        cfg.output = lox_span_writable(out, sizeof(out));

        r = loxguard_run(&cfg, probe_block, &probe);
        (void)r;
        failed |= expect(bb.count >= 3u, "loxguard_run bounds emits lifecycle events");
        failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "loxguard_run uses user-emitted bounds, not BLOCK_ERROR");
    }

    {
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        lox_report_t r;

        {
            const uint32_t script[] = { 10u, 60u };
            now_ms_set_script(script, sizeof(script) / sizeof(script[0]));
            lox_adapter_set_time_now(now_ms_from_script);
        }
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_OK;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "timeout_block";
        cfg.timeout_ms = 10u;
        cfg.blackbox = &bb;

        r = loxguard_run(&cfg, probe_block, &probe);
        failed |= expect(r.result == LOX_RESULT_TIMEOUT, "loxguard_run timeout -> report TIMEOUT");
        failed |= expect(bb.count >= 3u, "loxguard_run timeout emits lifecycle events");
        failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "loxguard_run timeout first event ENTERED");
        failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_TIMEOUT, "loxguard_run emits TIMEOUT event");
        failed |= expect(r.duration_ticks == 50u, "loxguard_run timeout duration tracked");
    }

    {
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        lox_report_t r;
        size_t before;

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_OK;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "cum_block";
        cfg.blackbox = &bb;

        (void)loxguard_run(&cfg, probe_block, &probe);
        before = bb.count;
        (void)loxguard_run(&cfg, probe_block, &probe);

        failed |= expect(bb.count > before, "loxguard_run does not reset caller blackbox (cumulative evidence)");
        r = loxguard_run(&cfg, probe_block, &probe);
        failed |= expect(r.result == LOX_RESULT_OK, "loxguard_run cumulative third call ok");
    }

    {
        /* Quarantine: after N failures, next call is blocked and fn is not invoked. */
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        size_t i;
        size_t before;

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "quarantine_block";
        cfg.max_failures = 3u;
        cfg.blackbox = &bb;

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_ERR_BOUNDS;

        for (i = 0u; i < 3u; i++) {
            (void)loxguard_run(&cfg, probe_block, &probe);
        }

        before = (size_t)probe.called;
        (void)loxguard_run(&cfg, probe_block, &probe);
        failed |= expect((size_t)probe.called == before, "loxguard_run quarantine does not call fn");
        failed |= expect(bb.count >= 1u && bb.events[bb.count - 2u].reason != NULL, "loxguard_run quarantine emits event");
        failed |= expect(bb.events[bb.count - 2u].reason != NULL && strcmp(bb.events[bb.count - 2u].reason, "QUARANTINED") == 0, "loxguard_run quarantine reason");
    }

    {
        /* Scratch arena handover and default block_name fallback. */
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        lox_report_t r;
        uint8_t scratch[32];

        g_now = 0u;
        {
            const uint32_t script[] = { 10u, 60u };
            now_ms_set_script(script, sizeof(script) / sizeof(script[0]));
            lox_adapter_set_time_now(now_ms_from_script);
        }
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = NULL; /* trigger fallback */
        cfg.blackbox = &bb;
        cfg.scratch = scratch;
        cfg.scratch_len = sizeof(scratch);

        r = loxguard_run(&cfg, scratch_block, NULL);
        failed |= expect(r.result == LOX_RESULT_OK, "loxguard_run scratch arena usable");
        failed |= expect(bb.count >= 3u, "loxguard_run scratch emits lifecycle events");
        failed |= expect(bb.events[0].block_name != NULL && strcmp(bb.events[0].block_name, "guard_block") == 0, "loxguard_run default block_name fallback");
        failed |= expect(r.duration_ticks == 50u, "loxguard_run duration tracked with scripted clock");
    }

    {
        /* Recovery callback is invoked on failure with policy action. */
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        recovery_probe_t rprobe;
        lox_report_t r;
        uint8_t out[4] = {0};

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        memset(&rprobe, 0, sizeof(rprobe));
        lox_set_recovery_callback(recovery_probe_cb, &rprobe);

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_ERR_BOUNDS;
        probe.emit_bounds = 1;

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "recovery_block";
        cfg.blackbox = &bb;
        cfg.output = lox_span_writable(out, sizeof(out));

        r = loxguard_run(&cfg, probe_block, &probe);
        (void)r;
        failed |= expect(rprobe.calls == 1, "loxguard_run invokes recovery callback on failure");
        failed |= expect(rprobe.last_kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "recovery callback sees incident kind");
        failed |= expect(rprobe.last_action != LOX_ACTION_NONE, "recovery callback receives policy action");

        lox_set_recovery_callback(NULL, NULL);
    }

#if defined(LOXGUARD_HAVE_MICRORES) && defined(LOXGUARD_USE_MICRORES)
    {
        /* Breaker open path (microres only): open breaker via repeated failure reports, then ensure loxguard_run blocks fn. */
        lox_blackbox_t bb;
        loxguard_block_cfg_t cfg;
        run_probe_t probe;
        int j;
        size_t before;

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_0);
        lox_adapter_recovery_reset();
        lox_blackbox_init(&bb);

        for (j = 0; j < 4; j++) {
            lox_adapter_recovery_report_result_for_block("breaker_block", 0);
        }

        memset(&cfg, 0, sizeof(cfg));
        cfg.name = "breaker_block";
        cfg.blackbox = &bb;

        memset(&probe, 0, sizeof(probe));
        probe.fail_rc = LOXGUARD_OK;

        before = (size_t)probe.called;
        (void)loxguard_run(&cfg, probe_block, &probe);
        failed |= expect((size_t)probe.called == before, "loxguard_run breaker-open does not call fn");
    }
#endif

    return failed;
}
