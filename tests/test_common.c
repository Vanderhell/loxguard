#include "test_common.h"

#include <stdio.h>

static int g_expect_total = 0;
static int g_expect_failed = 0;

void expect_reset(void) {
    g_expect_total = 0;
    g_expect_failed = 0;
}

int expect_fail_count(void) {
    return g_expect_failed;
}

int expect_total_count(void) {
    return g_expect_total;
}

void expect_print_summary(void) {
    printf("EXPECT: %d/%d passed\n", g_expect_total - g_expect_failed, g_expect_total);
}

int expect(int cond, const char *msg) {
    g_expect_total++;
    if (!cond) {
        printf("FAIL: %s\n", msg);
        g_expect_failed++;
        return 1;
    }
    return 0;
}
