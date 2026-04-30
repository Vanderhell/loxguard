#include "loxguard.h"

int lox_guarded_copy_first4(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len);
int lox_guarded_write_marker(uint8_t *out, size_t out_len, uint8_t value);

#include "test_common.h"

int test_guarded_module_suite(void) {
    uint8_t in[8] = {1,2,3,4,5,6,7,8};
    uint8_t out[8] = {0};
    int failed = 0;

    failed |= expect(lox_guarded_copy_first4(in, sizeof(in), out, sizeof(out)) == LOXGUARD_OK, "guarded copy first4 ok");
    failed |= expect(out[0] == 1u && out[3] == 4u, "guarded copy payload");
    failed |= expect(lox_guarded_copy_first4(in, 2u, out, sizeof(out)) == LOXGUARD_ERR_BOUNDS, "guarded copy bounds fail");

    failed |= expect(lox_guarded_write_marker(out, sizeof(out), 0x5Au) == LOXGUARD_OK, "guarded write marker ok");
    failed |= expect(out[0] == 0x5Au, "guarded write payload");

    return failed;
}
