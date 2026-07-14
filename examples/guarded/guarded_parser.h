#ifndef LOX_GUARDED_PARSER_H
#define LOX_GUARDED_PARSER_H

#include "loxguard.h"

int lox_guarded_copy_first4(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len);
int lox_guarded_write_marker(uint8_t *out, size_t out_len, uint8_t value);

#endif
