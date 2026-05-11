#include "test_common.h"

#include <stdio.h>

int test_pipeline_legacy_suite(void);

int test_pipeline_suite(void) {
    int failed = 0;

    failed |= test_pipeline_legacy_suite();

    if (!failed) {
        printf("PASS: pipeline legacy suite\n");
    }
    return failed;
}

