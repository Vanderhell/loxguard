#include "loxguard.h"
#include "loxguard_rtos_bridge.h"
#include "test_common.h"

int test_rtos_bridge_suite(void) {
    lox_blackbox_t bb;
    lox_rtos_task_probe_t probe;
    int failed = 0;

    lox_blackbox_init(&bb);

    probe.task_name = "task_a";
    probe.tick_budget = 10u;
    probe.elapsed_ticks = 9u;
    failed |= expect(lox_rtos_check_task_budget(&bb, &probe) == LOXGUARD_OK, "rtos bridge in-budget");
    failed |= expect(bb.count == 0u, "rtos bridge no event in-budget");

    probe.elapsed_ticks = 11u;
    failed |= expect(lox_rtos_check_task_budget(&bb, &probe) == LOXGUARD_OK, "rtos bridge timeout mapped");
    failed |= expect(bb.count == 1u, "rtos bridge timeout event count");
    failed |= expect(bb.events[0].kind == LOX_EVENT_BLOCK_TIMEOUT, "rtos bridge timeout kind");

    return failed;
}
