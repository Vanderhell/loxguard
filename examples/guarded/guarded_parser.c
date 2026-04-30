#include "loxguard.h"
#include "loxguard_checked.h"

#define LOX_GUARDED_FILE 1

int lox_guarded_copy_first4(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len) {
    lox_span_t src;
    lox_span_t dst;
    int rc;

    if (in == NULL || out == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    src = lox_span_readonly(in, in_len);
    dst = lox_span_writable(out, out_len);

    rc = LOX_MEMCPY(&dst, 0u, &src, 0u, 4u);
    return rc;
}

int lox_guarded_write_marker(uint8_t *out, size_t out_len, uint8_t value) {
    lox_span_t dst;

    if (out == NULL) {
        return LOXGUARD_ERR_NULL;
    }

    dst = lox_span_writable(out, out_len);
    return LOX_WRITE_U8(&dst, 0u, value);
}
