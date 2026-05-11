#include "test_common.h"

#include <stdio.h>

int main(void) {
    int failed = 0;

    expect_reset();
    failed |= test_span_suite();
    failed |= test_arena_suite();
    failed |= test_pipeline_suite();
    failed |= test_loxguard_run_suite();
    failed |= test_rtos_bridge_suite();
    failed |= test_guarded_module_suite();
    failed |= test_ports_suite();
    failed |= test_shell_suite();

    expect_print_summary();
    if (failed) {
        return 1;
    }

    printf("PASS: span/arena/pipeline/rtos/guarded/ports/shell test matrix\n");
    return 0;
}
