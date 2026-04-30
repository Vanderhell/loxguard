#include "loxguard.h"
#include "test_common.h"

int test_span_suite(void) {
    uint8_t buf[8] = {0,1,2,3,4,5,6,7};
    uint8_t out[4] = {0};
    uint8_t src[3] = {9,8,7};
    lox_span_t rw = lox_span_writable(buf, sizeof(buf));
    lox_span_t ro = lox_span_readonly(buf, sizeof(buf));
    int failed = 0;
    int rc;

    rc = lox_span_read_u8(&rw, 2u, &out[0]);
    failed |= expect(rc == LOXGUARD_OK && out[0] == 2u, "read_u8 in-bounds");

    rc = lox_span_read_u8(&rw, 8u, &out[0]);
    failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "read_u8 out-of-bounds");

    rc = lox_span_write_u8(&rw, 3u, 42u);
    failed |= expect(rc == LOXGUARD_OK && buf[3] == 42u, "write_u8 in-bounds");

    rc = lox_span_write_u8(&rw, 9u, 42u);
    failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "write_u8 out-of-bounds");

    rc = lox_span_write_u8(&ro, 0u, 99u);
    failed |= expect(rc == LOXGUARD_ERR_READONLY, "write_u8 readonly rejection");

    rc = lox_span_read(&rw, 1u, out, sizeof(out));
    failed |= expect(rc == LOXGUARD_OK, "span_read valid");
    failed |= expect(out[0] == 1u && out[3] == 4u, "span_read payload");

    rc = lox_span_read(&rw, 6u, out, 4u);
    failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "span_read invalid range");

    rc = lox_span_write(&rw, 2u, src, sizeof(src));
    failed |= expect(rc == LOXGUARD_OK, "span_write valid");
    failed |= expect(buf[2] == 9u && buf[4] == 7u, "span_write payload");

    rc = lox_span_write(&rw, 7u, src, sizeof(src));
    failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "span_write invalid range");

    {
        uint8_t src_buf[8] = {10,11,12,13,14,15,16,17};
        uint8_t dst_buf[8] = {0};
        lox_span_t s = lox_span_readonly(src_buf, sizeof(src_buf));
        lox_span_t d = lox_span_writable(dst_buf, sizeof(dst_buf));
        lox_span_t d_ro = lox_span_readonly(dst_buf, sizeof(dst_buf));

        rc = lox_span_memcpy(&d, 2u, &s, 1u, 4u);
        failed |= expect(rc == LOXGUARD_OK, "span_memcpy valid");
        failed |= expect(dst_buf[2] == 11u && dst_buf[5] == 14u, "span_memcpy payload");

        rc = lox_span_memcpy(&d, 6u, &s, 1u, 4u);
        failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "span_memcpy dst out-of-bounds");

        rc = lox_span_memcpy(&d, 0u, &s, 7u, 2u);
        failed |= expect(rc == LOXGUARD_ERR_BOUNDS, "span_memcpy src out-of-bounds");

        rc = lox_span_memcpy(&d_ro, 0u, &s, 0u, 1u);
        failed |= expect(rc == LOXGUARD_ERR_READONLY, "span_memcpy readonly dst rejection");
    }

    return failed;
}
