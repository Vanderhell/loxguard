#define MTEST_IMPLEMENTATION
#include "mtest.h"

MTEST(loxguard_microtest_smoke_case) {
    MTEST_ASSERT_EQ(1, 1);
}

MTEST_SUITE(loxguard_microtest_smoke_suite) {
    MTEST_RUN(loxguard_microtest_smoke_case);
}

int main(int argc, char **argv) {
    MTEST_BEGIN(argc, argv);
    MTEST_SUITE_RUN(loxguard_microtest_smoke_suite);
    return MTEST_END();
}
