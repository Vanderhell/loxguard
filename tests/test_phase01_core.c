#include "loxguard.h"
#include "loxguard_adapters.h"
#include "loxguard_checked.h"
#include "loxguard_format.h"
#include "test_common.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int g_arena_count_calls = 0;

static size_t phase01_count_expr(void) {
    g_arena_count_calls++;
    return 3u;
}

static uint32_t phase01_hash(const char *key) {
    const unsigned char *p = (const unsigned char *)key;
    uint32_t hash = 2166136261u;

    while (p != NULL && *p != '\0') {
        hash ^= (uint32_t)(*p++);
        hash *= 16777619u;
    }
    return hash;
}

static int check_report_legacy_null_texts(void) {
    lox_report_t report;
    lox_event_kind_t kind = LOX_EVENT_NONE;
    int failed = 0;

    memset(&report, 0xA5, sizeof(report));
    failed |= expect(
        lox_report_parse_kv("block=ok,reason=done,result=1,action=0,event_kind=7,duration_ticks=2,event_persisted=1", &report, &kind) == 1,
        "legacy report parse succeeds"
    );
    failed |= expect(kind == LOX_EVENT_BLOCK_OK, "legacy report parse returns event kind");
    failed |= expect(report.result == LOX_RESULT_OK, "legacy report parse copies numeric fields");
    failed |= expect(report.last_block == NULL, "legacy report parse clears last_block pointer");
    failed |= expect(report.last_failed_block == NULL, "legacy report parse clears last_failed_block pointer");
    failed |= expect(report.reason == NULL, "legacy report parse clears reason pointer");
    return failed;
}

static int check_report_strict_parser(void) {
    lox_report_snapshot_t snap;
    int failed = 0;

    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 1,
        "report parse accepts canonical line"
    );
    failed |= expect(strcmp(snap.report.last_block, "alpha") == 0, "report parse ex preserves block text");
    failed |= expect(strcmp(snap.report.reason, "beta") == 0, "report parse ex preserves reason text");

    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1,extra=2", &snap) == 0,
        "report parse rejects trailing field"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3,action=1,result=4,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 0,
        "report parse rejects duplicate field"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,action=1,result=3,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 0,
        "report parse rejects reordered field"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3x,action=1,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 0,
        "report parse rejects malformed numeric"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=4294967296,action=1,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 0,
        "report parse rejects numeric overflow"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1 ", &snap) == 0,
        "report parse rejects trailing whitespace"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1,", &snap) == 0,
        "report parse rejects trailing comma"
    );
    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta%00tail,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 0,
        "report parse rejects embedded NUL encoding"
    );

    failed |= expect(
        lox_report_parse_kv_ex("block=alpha,reason=beta%2Cgamma%3Ddelta%25,result=3,action=1,event_kind=3,duration_ticks=9,event_persisted=1", &snap) == 1,
        "report parse accepts percent encoding"
    );
    failed |= expect(strcmp(snap.report.reason, "beta,gamma=delta%") == 0, "report parse decodes percent encoding");

    return failed;
}

static int check_event_strict_parser(void) {
    lox_event_t event;
    lox_event_snapshot_t snap;
    int failed = 0;

    memset(&event, 0, sizeof(event));
    event.kind = LOX_EVENT_BLOCK_ERROR;
    event.block_name = "blk,one=two";
    event.reason = "why=three,four";
    event.index = 7u;
    event.limit = 9u;
    event.aux_code = 11u;
    (void)lox_event_format_csv(&event, (char[160]){0}, 160u);

    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,limit=9,aux=11", &snap) == 1,
        "event parse accepts canonical line"
    );
    failed |= expect(strcmp(snap.event.block_name, "alpha") == 0, "event parse ex preserves block text");
    failed |= expect(strcmp(snap.event.reason, "beta") == 0, "event parse ex preserves reason text");

    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,limit=9,aux=11,extra=1", &snap) == 0,
        "event parse rejects trailing field"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,index=8,limit=9,aux=11", &snap) == 0,
        "event parse rejects duplicate field"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,limit=9,index=7,aux=11", &snap) == 0,
        "event parse rejects reordered field"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,limit=9,aux=999999999999999999999", &snap) == 0,
        "event parse rejects numeric overflow"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,limit=9,aux=11 ", &snap) == 0,
        "event parse rejects trailing whitespace"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta,index=7,limit=9,aux=11,", &snap) == 0,
        "event parse rejects trailing comma"
    );
    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha,reason=beta%00tail,index=7,limit=9,aux=11", &snap) == 0,
        "event parse rejects embedded NUL encoding"
    );

    failed |= expect(
        lox_event_parse_csv_line_ex("kind=3,block=alpha%2Cbeta%3Dgamma%25,reason=delta%2Cepsilon%3Dzeta%25,index=7,limit=9,aux=11", &snap) == 1,
        "event parse accepts percent encoding"
    );
    failed |= expect(strcmp(snap.event.block_name, "alpha,beta=gamma%") == 0, "event parse decodes percent encoded block");
    failed |= expect(strcmp(snap.event.reason, "delta,epsilon=zeta%") == 0, "event parse decodes percent encoded reason");

    return failed;
}

static int check_arena_macro(void) {
    uint8_t mem[128];
    lox_arena_t arena;
    void *p_once;
    void *p_zero;
    void *p_overflow;
    uint8_t *m1;
    uint8_t *m2;
    uintptr_t addr;
    size_t align;
    int failed = 0;

    lox_arena_init(&arena, mem, sizeof(mem));
    g_arena_count_calls = 0;
    p_once = LOX_ARENA_ALLOC(&arena, uint8_t, phase01_count_expr());
    failed |= expect(g_arena_count_calls == 1, "arena macro evaluates count once");
    failed |= expect(p_once != NULL, "arena macro allocates valid object");

    p_zero = LOX_ARENA_ALLOC(&arena, uint8_t, 0u);
    failed |= expect(p_zero != NULL, "arena macro handles zero count");

    p_overflow = LOX_ARENA_ALLOC(&arena, uint64_t, (size_t)-1);
    failed |= expect(p_overflow == NULL, "arena macro rejects multiplication overflow");

    align = 16u;
    addr = (uintptr_t)mem;
    while (align < 64u && (addr % (align * 2u)) == 0u) {
        align *= 2u;
    }
    if (align > sizeof(void *)) {
        void *aligned = lox_arena_alloc(&arena, 1u, align);
        failed |= expect(aligned != NULL, "arena supports higher-than-pointer alignment when base allows it");
        if (aligned != NULL) {
            failed |= expect(((uintptr_t)aligned % align) == 0u, "arena returns requested high alignment");
        }
    }

    m1 = LOX_ARENA_ALLOC(&arena, uint8_t, 1u), m2 = LOX_ARENA_ALLOC(&arena, uint8_t, 1u);
    failed |= expect(m1 != NULL && m2 != NULL, "arena macro compiles on one line");

    return failed;
}

static int check_span_overlap(void) {
    uint8_t buf1[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
    uint8_t buf2[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
    lox_span_t span1 = lox_span_writable(buf1, sizeof(buf1));
    lox_span_t span2 = lox_span_writable(buf2, sizeof(buf2));
    int failed = 0;

    failed |= expect(lox_span_memcpy(&span1, 1u, &span1, 0u, 5u) == LOXGUARD_OK, "span memcpy handles forward overlap");
    failed |= expect(memcmp(buf1, "aabcde", 6u) == 0, "span memcpy forward overlap result");

    failed |= expect(lox_span_memcpy(&span2, 0u, &span2, 1u, 5u) == LOXGUARD_OK, "span memcpy handles backward overlap");
    failed |= expect(memcmp(buf2, "bcdeff", 6u) == 0, "span memcpy backward overlap result");

    return failed;
}

static int check_blackbox_and_block_state(void) {
    lox_blackbox_t bb;
    lox_event_t event;
    lox_event_t source;
    char block[80];
    char reason[80];
    char msg[160];
    lox_adapter_debug_block_state_t state;
    size_t slot_count;
    int failed = 0;

    lox_blackbox_init(&bb);
    lox_adapter_debug_reset_block_states();
    memset(&source, 0, sizeof(source));
    source.kind = LOX_EVENT_BLOCK_TIMEOUT;
    source.block_name = "temp_block";
    source.reason = "temp_reason";
    source.index = 3u;
    source.limit = 7u;
    source.aux_code = 9u;
    lox_blackbox_store(&bb, &source);
    failed |= expect(strcmp(bb.events[0].block_name, "temp_block") == 0, "blackbox copies block text");
    failed |= expect(strcmp(bb.events[0].reason, "temp_reason") == 0, "blackbox copies reason text");

    bb.count = (sizeof(bb.events) / sizeof(bb.events[0])) + 4u;
    event.kind = LOX_EVENT_BLOCK_ERROR;
    event.block_name = "rollover";
    event.reason = "rollover";
    event.index = 42u;
    event.limit = 43u;
    event.aux_code = 44u;
    lox_blackbox_store(&bb, &event);
    failed |= expect(bb.count == (sizeof(bb.events) / sizeof(bb.events[0])), "blackbox clamps corrupted count");
    failed |= expect(strcmp(bb.events[bb.count - 1u].block_name, "rollover") == 0, "blackbox rollover retains latest event");

    lox_adapter_health_set_for_block("phase1_slot_0", 1);
    lox_adapter_health_set_for_block("phase1_slot_1", 2);
    lox_adapter_health_set_for_block("phase1_slot_2", 3);
    lox_adapter_health_set_for_block("phase1_slot_3", 4);
    lox_adapter_health_set_for_block("phase1_slot_4", 5);
    lox_adapter_health_set_for_block("phase1_slot_5", 6);
    lox_adapter_health_set_for_block("phase1_slot_6", 7);
    lox_adapter_health_set_for_block("phase1_slot_7", 8);
    lox_adapter_health_set_for_block("phase1_slot_8", 9);
    slot_count = lox_adapter_debug_block_state_count();
    failed |= expect(slot_count >= 9u, "block state table exposes overflow slot");
    {
        int found_slot0 = 0;
        int found_slot8 = 0;
        for (size_t i = 0u; i < slot_count; i++) {
            if (!lox_adapter_debug_block_state_snapshot(i, &state) || !state.in_use) {
                continue;
            }
            if (strcmp(state.block_name, "phase1_slot_0") == 0) {
                found_slot0 = 1;
                failed |= expect(state.health_code == 1, "block state slot 0 preserved");
            }
            if (strcmp(state.block_name, "phase1_slot_8") == 0) {
                found_slot8 = 1;
                failed |= expect(state.health_code == 9, "block state overflow slot preserved");
            }
        }
        failed |= expect(found_slot0, "block state slot 0 present");
        failed |= expect(found_slot8, "block state overflow slot present");
    }

    memset(block, 'A', 70u);
    block[70] = '1';
    block[71] = '\0';
    memset(reason, 'B', 70u);
    reason[70] = '2';
    reason[71] = '\0';
    lox_adapter_health_set_for_block(block, 11);
    {
        uint32_t hash_block = phase01_hash(block);
        int found_block = 0;

        for (size_t i = 0u; i < slot_count; i++) {
            if (!lox_adapter_debug_block_state_snapshot(i, &state) || !state.in_use) {
                continue;
            }
            if (state.name_hash == hash_block) {
                found_block = 1;
                failed |= expect(state.health_code == 11, "block state distinguishes long keys by hash");
            }
        }
        failed |= expect(found_block, "block state long key one present");
    }

    lox_adapter_debug_reset_block_states();
    lox_adapter_health_set_for_block(reason, 12);
    {
        uint32_t hash_reason = phase01_hash(reason);
        int found_reason = 0;

        for (size_t i = 0u; i < slot_count; i++) {
            if (!lox_adapter_debug_block_state_snapshot(i, &state) || !state.in_use) {
                continue;
            }
            if (state.name_hash == hash_reason) {
                found_reason = 1;
                failed |= expect(state.health_code == 12, "block state separates long key collision candidates");
            }
        }
        failed |= expect(found_reason, "block state long key two present");
    }

    lox_adapter_debug_reset_block_states();

    return failed;
}

int test_phase01_core_regressions_suite(void) {
    int failed = 0;

    failed |= check_report_legacy_null_texts();
    failed |= check_report_strict_parser();
    failed |= check_event_strict_parser();
    failed |= check_arena_macro();
    failed |= check_span_overlap();
    failed |= check_blackbox_and_block_state();

    if (!failed) {
        printf("PASS: phase01 core regressions suite\n");
    }
    return failed;
}
