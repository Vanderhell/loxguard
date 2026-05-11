#include "loxguard.h"
#include "loxguard_adapters.h"
#include "test_common.h"

#include <string.h>

typedef struct {
    int called;
    int fail_rc;
    int emit_bounds;
} run_probe_t;

static uint32_t g_now = 0u;
static uint32_t now_ms_inc_0(void) { return g_now; }
static uint32_t now_ms_inc_50(void) { g_now += 50u; return g_now; }

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

        g_now = 0u;
        lox_adapter_set_time_now(now_ms_inc_50);
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
        failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_TIMEOUT, "loxguard_run emits TIMEOUT event");
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

