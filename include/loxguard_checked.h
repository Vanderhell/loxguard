#ifndef LOXGUARD_CHECKED_H
#define LOXGUARD_CHECKED_H

#include "loxguard.h"

#if defined(__clang__) || defined(__GNUC__)
#define LOXGUARD_ALIGNOF(type) __alignof__(type)
#elif defined(_MSC_VER)
#define LOXGUARD_ALIGNOF(type) __alignof(type)
#else
#define LOXGUARD_ALIGNOF(type) offsetof(struct { char _loxguard_align; type _loxguard_member; }, _loxguard_member)
#endif

#define LOX_READ_U8(span_ptr, idx, out_ptr) lox_span_read_u8((span_ptr), (idx), (out_ptr))
#define LOX_WRITE_U8(span_ptr, idx, val) lox_span_write_u8((span_ptr), (idx), (val))
#define LOX_MEMCPY(dst_ptr, dst_off, src_ptr, src_off, len) \
    lox_span_memcpy((dst_ptr), (dst_off), (src_ptr), (src_off), (len))
#define LOX_ARENA_ALLOC(arena_ptr, type, count) \
    ((type *)loxguard_arena_alloc_array((arena_ptr), sizeof(type), (size_t)(count), LOXGUARD_ALIGNOF(type)))

#endif
