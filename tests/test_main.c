#include "test_common.h"

#include <stdio.h>

int main(void) {
    int failed = 0;

    failed |= test_span_suite();
    failed |= test_arena_suite();
    failed |= test_pipeline_suite();
    failed |= test_rtos_bridge_suite();
    failed |= test_guarded_module_suite();
    failed |= test_ports_suite();

    if (failed) {
        return 1;
    }

    printf("PASS: span/arena/pipeline/rtos/guarded/ports test matrix\n");
    return 0;
}
