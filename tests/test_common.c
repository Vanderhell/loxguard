#include "test_common.h"

#include <stdio.h>

int expect(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        return 1;
    }
    return 0;
}
