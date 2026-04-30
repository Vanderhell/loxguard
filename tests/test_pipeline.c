#include "loxguard.h"
#include "loxguard_adapters.h"
#include "loxguard_format.h"
#include "loxguard_ports.h"
#include "test_common.h"

#include <stdio.h>
#include <string.h>

static int recovery_calls = 0;
static lox_action_t last_action = LOX_ACTION_NONE;
static lox_event_kind_t last_kind = LOX_EVENT_NONE;

static uint32_t fake_now = 0u;

static uint32_t fake_now_ms(void) {
    fake_now += 5u;
    return fake_now;
}

static uint32_t lcg_next(uint32_t *state) {
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

typedef struct {
    size_t count;
    char last_line[160];
} export_capture_t;

static void recovery_probe(const lox_event_t *event, lox_action_t action, void *user_ctx) {
    (void)user_ctx;
    recovery_calls++;
    last_action = action;
    last_kind = (event == NULL) ? LOX_EVENT_NONE : event->kind;
}

static void capture_line(const char *line, void *user_ctx) {
    export_capture_t *cap = (export_capture_t *)user_ctx;
    if (cap == NULL || line == NULL) {
        return;
    }
    cap->count++;
    strncpy(cap->last_line, line, sizeof(cap->last_line) - 1u);
    cap->last_line[sizeof(cap->last_line) - 1u] = '\0';
}

int test_pipeline_suite(void) {
    uint8_t in[8] = {0,1,2,3,4,5,6,7};
    uint8_t out_ok[32];
    uint8_t out_fail[16];
    uint8_t scratch_ok[64];
    uint8_t scratch_small[4];
    lox_blackbox_t bb;
    lox_report_t report;
    lox_event_t parsed;
    lox_event_snapshot_t parsed_event_snapshot;
    lox_report_t parsed_report;
    lox_event_kind_t parsed_kind;
    lox_report_snapshot_t parsed_snapshot;
    lox_blackbox_t imported_bb;
    lox_blackbox_t ownership_bb;
    lox_event_t ownership_event;
    char mutable_block[32];
    char mutable_reason[32];
    lox_blackbox_t rollover_bb;
    lox_event_t rollover_event;
    char rollover_block[32];
    char rollover_reason[32];
    char long_block[128];
    char long_reason[128];
    char line[160];
    char dump[512];
    char dump_h[768];
    char tiny[24];
    char report_line[256];
    char mixed_import[512];
    char fuzz_line[192];
    size_t incident_idx;
    export_capture_t cap;
    lox_event_t policy_probe;
    int failed = 0;
    lox_event_t persist_probe;
    uint32_t fuzz_state;

    lox_adapter_set_time_now(fake_now_ms);
    lox_adapter_watchdog_reset();
    lox_adapter_recovery_reset();
    lox_adapter_bus_reset_stats();
    lox_set_recovery_callback(recovery_probe, NULL);
    memset(&persist_probe, 0, sizeof(persist_probe));
    persist_probe.kind = LOX_EVENT_BLOCK_OK;
    persist_probe.block_name = "persist_probe";
    persist_probe.reason = "OK";

    memset(&policy_probe, 0, sizeof(policy_probe));
    policy_probe.kind = LOX_EVENT_NONE;
    failed |= expect(lox_policy_decide(&policy_probe) == LOX_ACTION_RESET_BLOCK, "policy reset-block for non-failure kind");

    lox_blackbox_init(&ownership_bb);
    strcpy(mutable_block, "temp_block");
    strcpy(mutable_reason, "temp_reason");
    memset(&ownership_event, 0, sizeof(ownership_event));
    ownership_event.kind = LOX_EVENT_BLOCK_TIMEOUT;
    ownership_event.block_name = mutable_block;
    ownership_event.reason = mutable_reason;
    ownership_event.index = 9u;
    ownership_event.limit = 11u;
    ownership_event.aux_code = 13u;
    lox_blackbox_store(&ownership_bb, &ownership_event);
    strcpy(mutable_block, "mutated_block");
    strcpy(mutable_reason, "mutated_reason");
    failed |= expect(strcmp(ownership_bb.events[0].block_name, "temp_block") == 0, "blackbox owns copied block text");
    failed |= expect(strcmp(ownership_bb.events[0].reason, "temp_reason") == 0, "blackbox owns copied reason text");

    memset(long_block, 'B', sizeof(long_block));
    long_block[sizeof(long_block) - 1u] = '\0';
    memset(long_reason, 'R', sizeof(long_reason));
    long_reason[sizeof(long_reason) - 1u] = '\0';
    ownership_event.block_name = long_block;
    ownership_event.reason = long_reason;
    lox_blackbox_store(&ownership_bb, &ownership_event);
    failed |= expect(strlen(ownership_bb.events[1].block_name) == 63u, "blackbox truncates long block to 63");
    failed |= expect(strlen(ownership_bb.events[1].reason) == 63u, "blackbox truncates long reason to 63");

    lox_blackbox_init(&rollover_bb);
    memset(&rollover_event, 0, sizeof(rollover_event));
    rollover_event.kind = LOX_EVENT_BLOCK_TIMEOUT;
    for (size_t i = 0u; i < 20u; i++) {
        (void)snprintf(rollover_block, sizeof(rollover_block), "blk_%zu", i);
        (void)snprintf(rollover_reason, sizeof(rollover_reason), "rsn_%zu", i);
        rollover_event.block_name = rollover_block;
        rollover_event.reason = rollover_reason;
        rollover_event.index = i;
        lox_blackbox_store(&rollover_bb, &rollover_event);
    }
    failed |= expect(rollover_bb.count == 16u, "blackbox rollover keeps bounded count");
    failed |= expect(strcmp(rollover_bb.events[0].block_name, "blk_4") == 0, "blackbox rollover oldest retained block");
    failed |= expect(strcmp(rollover_bb.events[15].block_name, "blk_19") == 0, "blackbox rollover newest block");
    for (size_t i = 20u; i < 2020u; i++) {
        (void)snprintf(rollover_block, sizeof(rollover_block), "blk_%zu", i);
        (void)snprintf(rollover_reason, sizeof(rollover_reason), "rsn_%zu", i);
        rollover_event.block_name = rollover_block;
        rollover_event.reason = rollover_reason;
        rollover_event.index = i;
        lox_blackbox_store(&rollover_bb, &rollover_event);
    }
    failed |= expect(rollover_bb.count == 16u, "blackbox long stress keeps bounded count");
    failed |= expect(strcmp(rollover_bb.events[0].block_name, "blk_2004") == 0, "blackbox long stress oldest retained block");
    failed |= expect(strcmp(rollover_bb.events[15].block_name, "blk_2019") == 0, "blackbox long stress newest block");

    memset(out_ok, 0x33, sizeof(out_ok));
    report = lox_run_checked_parser_demo(in, sizeof(in), out_ok, sizeof(out_ok), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(report.result == LOX_RESULT_OK, "success result is OK");
    failed |= expect(report.action == LOX_ACTION_NONE, "success action NONE");
    failed |= expect(report.last_failed_block == NULL, "success has no failed block");
    failed |= expect(bb.count >= 3u, "success has entered/ok/completed events");
    failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "success entered event");
    failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_OK, "success ok event");
    failed |= expect(bb.events[2].kind == LOX_EVENT_BLOCK_COMPLETED, "success completed event");
    failed |= expect(lox_adapter_health_get() == 0, "health state OK after successful block");
    failed |= expect(lox_adapter_watchdog_state_get() == 0, "watchdog state OK after successful block");
    if (lox_adapter_bus_publish_count() > 0u) {
        failed |= expect(lox_adapter_bus_publish_count() >= 3u, "microbus publishes success lifecycle events");
        failed |= expect(lox_adapter_bus_last_kind() == (uint8_t)LOX_EVENT_BLOCK_COMPLETED, "microbus last kind after success");
    } else {
        failed |= expect(lox_adapter_bus_publish_count() == 0u, "microbus disabled fallback count");
    }

    recovery_calls = 0;
    memset(out_fail, 0x11, sizeof(out_fail));
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(report.last_block != NULL, "report.last_block set");
    failed |= expect(strcmp(report.last_block, "packet_parser") == 0, "last block is packet_parser");
    failed |= expect(report.last_failed_block != NULL, "report.last_failed_block set");
    failed |= expect(strcmp(report.reason, "BOUNDS") == 0, "reason is BOUNDS");
    failed |= expect(report.result == LOX_RESULT_BOUNDS, "result is BOUNDS");
    failed |= expect(report.action == LOX_ACTION_DROP_INPUT, "policy action is DROP_INPUT");
    failed |= expect(report.duration_ticks > 0u, "duration ticks captured");
    failed |= expect((report.duration_ticks % 5u) == 0u, "duration ticks deterministic from injected clock");
    incident_idx = bb.count - 2u;
    failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "first event is ENTERED");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "incident event kind is OOB write");
    failed |= expect(bb.events[bb.count - 1u].kind == LOX_EVENT_BLOCK_COMPLETED, "last event is COMPLETED");
    failed |= expect(out_fail[15] == 0x11u, "buffer tail unchanged, no OOB write happened");
    failed |= expect(recovery_calls == 1, "recovery callback called for bounds");
    failed |= expect(last_action == LOX_ACTION_DROP_INPUT, "recovery action on bounds");
    failed |= expect(last_kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "recovery kind on bounds");
    failed |= expect(lox_adapter_watchdog_state_get() != 0, "watchdog state non-OK on bounds failure mapping");
    if (lox_adapter_bus_publish_count() > 0u) {
        failed |= expect(lox_adapter_bus_publish_count() >= 6u, "microbus publishes failure lifecycle events");
        failed |= expect(lox_adapter_bus_last_kind() == (uint8_t)LOX_EVENT_BLOCK_COMPLETED, "microbus last kind after failure");
    }

    (void)lox_event_format_csv(&bb.events[incident_idx], line, sizeof(line));
    failed |= expect(strstr(line, "reason=BOUNDS") != NULL, "csv export contains BOUNDS");
    failed |= expect(lox_event_parse_csv_line(line, &parsed) == 1, "csv parse BOUNDS line");
    failed |= expect(parsed.kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS, "csv parse kind round-trip");
    failed |= expect(lox_event_parse_csv_line_ex(line, &parsed_event_snapshot) == 1, "csv parse ex BOUNDS line");
    failed |= expect(strcmp(parsed_event_snapshot.event.block_name, "packet_parser") == 0, "csv parse ex block");

    cap.count = 0u;
    cap.last_line[0] = '\0';
    failed |= expect(lox_blackbox_export_csv_lines(&bb, 3u, capture_line, &cap) == 3u, "csv lines export three");
    failed |= expect(cap.count == 3u, "csv lines callback count three");

    recovery_calls = 0;
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_small, sizeof(scratch_small), &bb);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "ARENA") == 0, "arena overflow reason is ARENA");
    failed |= expect(report.result == LOX_RESULT_ARENA, "arena result");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_ARENA_OVERFLOW, "arena overflow incident kind");
    failed |= expect(lox_adapter_health_get() != 0, "health state not-OK after arena failure");

    recovery_calls = 0;
    report = lox_run_checked_parser_timeout_demo(&bb);
    incident_idx = bb.count - 2u;
    failed |= expect(report.result == LOX_RESULT_TIMEOUT, "timeout result");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_TIMEOUT, "timeout incident kind");
    failed |= expect(lox_adapter_health_get() != 0, "health state not-OK after timeout");
    failed |= expect(lox_adapter_watchdog_state_get() != 0, "watchdog state non-OK on timeout mapping");

    recovery_calls = 0;
    report = lox_run_guard_panic_demo(&bb);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "PANIC") == 0, "panic demo reason");
    failed |= expect(report.result == LOX_RESULT_ERROR, "panic demo result");
    failed |= expect(report.action == LOX_ACTION_DROP_INPUT, "panic demo action");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_PANIC, "panic demo incident kind");
    if (lox_adapter_bus_publish_count() > 0u) {
        failed |= expect(lox_adapter_bus_last_kind() == (uint8_t)LOX_EVENT_BLOCK_COMPLETED, "microbus panic flow completes");
    }

    recovery_calls = 0;
    report = lox_run_guard_fault_demo(&bb);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "FAULT") == 0, "fault demo reason");
    failed |= expect(report.result == LOX_RESULT_ERROR, "fault demo result");
    failed |= expect(report.action == LOX_ACTION_DROP_INPUT, "fault demo action");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_FAULT, "fault demo incident kind");

    recovery_calls = 0;
    report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 77u);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "RTOS_UNSUPPORTED") == 0, "rtos unsupported reason on host");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_UNSUPPORTED, "rtos unsupported incident kind");

    failed |= expect(lox_port_set_active(LOX_PORT_FREERTOS_STUB) == 0, "set freertos port");
    recovery_calls = 0;
    report = lox_run_rtos_timeout_demo(&bb, "rtos_task_demo", 77u);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "RTOS_TIMEOUT") == 0, "rtos demo reason");
    failed |= expect(report.result == LOX_RESULT_TIMEOUT, "rtos demo result timeout");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_TIMEOUT, "rtos demo incident kind");

    (void)lox_blackbox_export_csv_buffer(&bb, 3u, dump, sizeof(dump));
    failed |= expect(strstr(dump, "reason=RTOS_TIMEOUT") != NULL, "csv buffer contains RTOS_TIMEOUT");
    failed |= expect(strstr(dump, "reason=COMPLETED") != NULL, "csv buffer contains COMPLETED");

    (void)lox_blackbox_export_csv_buffer_ex(&bb, 3u, 1, dump_h, sizeof(dump_h));
    failed |= expect(lox_csv_schema_is_compatible(dump_h) == 1, "csv schema compatibility positive");
    failed |= expect(lox_blackbox_import_csv_buffer(dump_h, &imported_bb) == 3u, "csv import count from header dump");
    failed |= expect(strcmp(imported_bb.events[1].block_name, "rtos_task_demo") == 0, "csv import block text");

    (void)lox_report_format_kv(&report, &bb.events[incident_idx], report_line, sizeof(report_line));
    failed |= expect(strstr(report_line, "result=3") != NULL, "report format result");
    failed |= expect(strstr(report_line, "event_persisted=") != NULL, "report format persisted field");
    failed |= expect(strstr(report_line, "event_persisted=0") != NULL, "report format persisted disabled");
    failed |= expect(lox_report_parse_kv(report_line, &parsed_report, &parsed_kind) == 1, "report parse line");
    failed |= expect(parsed_report.result == LOX_RESULT_TIMEOUT, "report parse result");
    failed |= expect(parsed_kind == LOX_EVENT_BLOCK_TIMEOUT, "report parse event kind");
    failed |= expect(lox_report_parse_kv_ex(report_line, &parsed_snapshot) == 1, "report parse ex line");
    failed |= expect(strcmp(parsed_snapshot.report.reason, "RTOS_TIMEOUT") == 0, "report parse ex reason");

    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1,event_kind=3,duration_ticks=1,event_persisted=1", &parsed_report, &parsed_kind) == 1, "report parse accepts complete schema");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1", &parsed_report, &parsed_kind) == 0, "report parse rejects missing event_kind");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=99,event_kind=3,duration_ticks=1,event_persisted=1", &parsed_report, &parsed_kind) == 0, "report parse rejects out-of-range action");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1,event_kind=99,duration_ticks=1,event_persisted=1", &parsed_report, &parsed_kind) == 0, "report parse rejects out-of-range event kind");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1,event_kind=3,duration_ticks=1,event_persisted=2", &parsed_report, &parsed_kind) == 0, "report parse rejects invalid persisted flag");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1,event_kind=3,duration_ticks=1,event_persisted=1 ", &parsed_report, &parsed_kind) == 0, "report parse rejects trailing whitespace");
    failed |= expect(lox_report_parse_kv("block=x,reason=y,result=3,action=1,event_kind=3,duration_ticks=1,event_persisted=-1", &parsed_report, &parsed_kind) == 0, "report parse rejects negative persisted");

#if defined(LOXGUARD_USE_NVLOG) && defined(LOXGUARD_HAVE_NVLOG)
    failed |= expect(lox_adapter_nvlog_init_ram(64u) != LOXGUARD_OK, "nvlog init rejects undersized region");
    failed |= expect(lox_adapter_nvlog_init_file(NULL, 4096u) == LOXGUARD_ERR_NULL, "nvlog init file null path rejected");
    failed |= expect(lox_adapter_nvlog_init_ram(4096u) == LOXGUARD_OK, "nvlog init ram");
    lox_adapter_nvlog_inject_fail_after(-1);
    failed |= expect(lox_adapter_persist_event(&persist_probe) == LOXGUARD_OK, "persist success on nvlog");
    report = lox_run_checked_parser_demo(in, sizeof(in), out_ok, sizeof(out_ok), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(report.event_persisted == 1, "report persisted true with nvlog initialized");
    lox_adapter_nvlog_inject_fail_after(0);
    failed |= expect(lox_adapter_persist_event(&persist_probe) != LOXGUARD_OK, "persist failure on injected nvlog fault");
    lox_adapter_nvlog_shutdown();
    failed |= expect(lox_adapter_persist_event(&persist_probe) != LOXGUARD_OK, "persist unsupported after nvlog shutdown");
#else
    failed |= expect(lox_adapter_persist_event(&persist_probe) != LOXGUARD_OK, "persist fallback when nvlog disabled");
#endif

#if defined(LOXGUARD_USE_LOXDB) && defined(LOXGUARD_HAVE_LOXDB)
    failed |= expect(lox_adapter_persist_event(&persist_probe) == LOXGUARD_OK, "persist success on loxdb fallback");
    report = lox_run_checked_parser_demo(in, sizeof(in), out_ok, sizeof(out_ok), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(report.event_persisted == 1, "report persisted true with loxdb fallback");
#endif

    failed |= expect(lox_event_parse_csv_line_ex("kind=99,block=x,reason=y,index=1,limit=2,aux=3", &parsed_event_snapshot) == 0, "csv parse ex rejects out-of-range kind");
    failed |= expect(lox_event_parse_csv_line_ex("kind=3,block=x,reason=y,index=1,limit=2,aux=3,extra=z", &parsed_event_snapshot) == 0, "csv parse ex rejects trailing garbage");
    failed |= expect(lox_event_parse_csv_line_ex("kind=3,block=x,reason=y,index=1,limit=2,aux=3 ", &parsed_event_snapshot) == 0, "csv parse ex rejects trailing whitespace");
    failed |= expect(lox_event_parse_csv_line_ex("kind=3,block=x,reason=y,index=,limit=2,aux=3", &parsed_event_snapshot) == 0, "csv parse ex rejects empty numeric token");

    (void)snprintf(
        mixed_import,
        sizeof(mixed_import),
        "kind,block,reason,index,limit,aux\n"
        "kind=3,block=ok_a,reason=RA,index=1,limit=2,aux=3\n"
        "kind=99,block=bad,reason=RB,index=4,limit=5,aux=6\n"
        "kind=3,block=ok_b,reason=RC,index=7,limit=8,aux=9\n"
        "kind=3,block=bad_tail,reason=RD,index=10,limit=11,aux=12,extra=z\n"
    );
    failed |= expect(lox_blackbox_import_csv_buffer(mixed_import, &imported_bb) == 2u, "csv import skips invalid lines");
    failed |= expect(imported_bb.count == 2u, "csv import mixed count");
    failed |= expect(strcmp(imported_bb.events[0].block_name, "ok_a") == 0, "csv import mixed first block");
    failed |= expect(strcmp(imported_bb.events[1].block_name, "ok_b") == 0, "csv import mixed second block");

#if defined(LOXGUARD_USE_MICRORES) && defined(LOXGUARD_HAVE_MICRORES)
    size_t before_count;
    int saw_oob = 0;
    lox_event_t block_b_ev;
    lox_adapter_recovery_reset();
    memset(out_fail, 0x22, sizeof(out_fail));
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(strcmp(report.reason, "BOUNDS") == 0, "microres fail #1 reason");
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(strcmp(report.reason, "BOUNDS") == 0, "microres fail #2 reason");
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(strcmp(report.reason, "BOUNDS") == 0, "microres fail #3 reason");
    before_count = bb.count;
    report = lox_run_checked_parser_demo(in, sizeof(in), out_fail, sizeof(out_fail), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(strcmp(report.reason, "BREAKER_OPEN") == 0, "microres breaker open reason");
    failed |= expect(lox_adapter_recovery_state_get() != 0, "microres recovery state non-closed when breaker open");
    failed |= expect(lox_adapter_recovery_state_get_for_block("packet_parser") != 0, "block A breaker state open/half-open");
    failed |= expect(lox_adapter_recovery_allow_attempt_for_block("packet_parser") == 0, "block A is blocked");
    failed |= expect(lox_adapter_recovery_allow_attempt_for_block("telemetry") == 1, "block B is not blocked by block A");
    failed |= expect(bb.count >= 3u, "breaker-open flow has entered/error/completed");
    failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_ENTERED, "breaker-open first event entered");
    failed |= expect(bb.events[1].kind == LOX_EVENT_BLOCK_ERROR, "breaker-open incident event is block error");
    failed |= expect(strcmp(bb.events[1].reason, "BREAKER_OPEN") == 0, "breaker-open event reason");
    failed |= expect(bb.events[bb.count - 1u].kind == LOX_EVENT_BLOCK_COMPLETED, "breaker-open last event completed");
    failed |= expect(before_count == bb.count, "breaker-open path does not execute guarded write path");
    failed |= expect(out_fail[15] == 0x22u, "breaker-open keeps output unchanged");
    for (size_t i = 0u; i < bb.count; i++) {
        if (bb.events[i].kind == LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS) {
            saw_oob = 1;
            break;
        }
    }
    failed |= expect(saw_oob == 0, "breaker-open flow records no OOB incident");

    memset(&block_b_ev, 0, sizeof(block_b_ev));
    block_b_ev.kind = LOX_EVENT_BLOCK_ENTERED;
    block_b_ev.block_name = "telemetry";
    block_b_ev.reason = "ENTERED";
    lox_adapter_watchdog_observe_event(&block_b_ev);
    block_b_ev.kind = LOX_EVENT_BLOCK_OK;
    block_b_ev.reason = "OK";
    lox_adapter_watchdog_observe_event(&block_b_ev);
    lox_adapter_recovery_report_result_for_block("telemetry", 1);
    lox_adapter_health_set_for_block("telemetry", 0);
    failed |= expect(lox_adapter_watchdog_state_get_for_block("telemetry") == 0, "block B watchdog OK");
    failed |= expect(lox_adapter_health_get_for_block("telemetry") == 0, "block B health OK");
    failed |= expect(lox_adapter_recovery_state_get_for_block("packet_parser") != 0, "block B success does not reset block A breaker");

    for (size_t i = 0u; i < 300u; i++) {
        (void)lox_adapter_now_ms();
    }
    report = lox_run_checked_parser_demo(in, sizeof(in), out_ok, sizeof(out_ok), scratch_ok, sizeof(scratch_ok), &bb);
    failed |= expect(report.result == LOX_RESULT_OK, "microres recovery allows success after open timeout");
    failed |= expect(lox_adapter_recovery_state_get() == 0, "microres state closes after success");
    failed |= expect(lox_adapter_recovery_state_get_for_block("packet_parser") == 0, "block A breaker closes after success");
#endif

    fuzz_state = 0xC0FFEEu;
    for (size_t i = 0u; i < 64u; i++) {
        uint32_t r = lcg_next(&fuzz_state);
        const char *prefix = (r & 1u) ? "kind=" : "knd=";
        const char *suffix = (r & 2u) ? ",tail=Z" : "";
        int n = snprintf(
            fuzz_line,
            sizeof(fuzz_line),
            "%s%u,block=B%u,reason=%s,index=%u,limit=%u,aux=%u%s",
            prefix,
            (unsigned int)(r % 32u),
            (unsigned int)(r % 100u),
            (r & 4u) ? "R" : "",
            (unsigned int)(r % 9u),
            (unsigned int)(r % 9u),
            (unsigned int)(r % 9u),
            suffix
        );
        failed |= expect(n > 0, "csv fuzz line generation");
        if (strcmp(prefix, "kind=") != 0 || strlen(suffix) != 0u || strstr(fuzz_line, "reason=,") != NULL) {
            failed |= expect(lox_event_parse_csv_line_ex(fuzz_line, &parsed_event_snapshot) == 0, "csv fuzz invalid mutation rejected");
        }
    }

    (void)lox_blackbox_export_csv_buffer(&bb, 3u, tiny, sizeof(tiny));
    failed |= expect(tiny[sizeof(tiny) - 1u] == '\0', "csv buffer truncation keeps null-termination");
    failed |= expect(lox_port_set_active(LOX_PORT_HOST) == 0, "restore host port after rtos");

    recovery_calls = 0;
    report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "MPU_UNSUPPORTED") == 0, "mpu unsupported reason on host");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_UNSUPPORTED, "mpu unsupported incident kind");

    failed |= expect(lox_port_set_active(LOX_PORT_CORTEXM_STUB) == 0, "set cortexm port");
    recovery_calls = 0;
    report = lox_run_mpu_fault_demo(&bb, "mpu_demo", 0x20000020u, 0x12u, 0x0u);
    incident_idx = bb.count - 2u;
    failed |= expect(strcmp(report.reason, "MPU_FAULT") == 0, "mpu demo reason");
    failed |= expect(report.result == LOX_RESULT_MEMORY_FAULT, "mpu result memory fault");
    failed |= expect(bb.events[incident_idx].kind == LOX_EVENT_BLOCK_MEMORY_FAULT, "mpu demo incident kind");
    failed |= expect(lox_port_set_active(LOX_PORT_HOST) == 0, "restore host port after mpu");

    lox_set_recovery_callback(NULL, NULL);
    return failed;
}
