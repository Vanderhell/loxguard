# Safe Span and Arena API

## 1. Purpose

The span/arena API is the MVP implementation path for Checked Guard Blocks.

It avoids needing a compiler plugin in the first version.

## 2. Span type

```c
typedef struct lox_span {
    uint8_t *base;
    size_t len;
    uint32_t flags;
} lox_span_t;
```

## 3. Span functions

Initial API:

```c
lox_span_t lox_span_readonly(const void *ptr, size_t len);
lox_span_t lox_span_writable(void *ptr, size_t len);

int lox_span_read_u8(const lox_span_t *s, size_t index, uint8_t *out);
int lox_span_write_u8(lox_span_t *s, size_t index, uint8_t value);

int lox_span_read(const lox_span_t *s, size_t offset, void *out, size_t len);
int lox_span_write(lox_span_t *s, size_t offset, const void *src, size_t len);

int lox_span_memcpy(lox_span_t *dst, size_t dst_off,
                    const lox_span_t *src, size_t src_off,
                    size_t len);
```

## 4. Arena type

```c
typedef struct lox_arena {
    uint8_t *base;
    size_t size;
    size_t used;
} lox_arena_t;
```

## 5. Arena functions

```c
void lox_arena_init(lox_arena_t *a, void *mem, size_t size);
void *lox_arena_alloc(lox_arena_t *a, size_t size, size_t align);
void lox_arena_reset(lox_arena_t *a);
size_t lox_arena_used(const lox_arena_t *a);
size_t lox_arena_remaining(const lox_arena_t *a);
```

## 6. Error codes

```text
LOXGUARD_OK
LOXGUARD_ERR_BOUNDS
LOXGUARD_ERR_NULL
LOXGUARD_ERR_READONLY
LOXGUARD_ERR_OVERFLOW
LOXGUARD_ERR_ALIGN
LOXGUARD_ERR_ARENA_FULL
```

## 7. Requirements

- C99.
- no heap.
- no external dependencies.
- checked integer arithmetic, ideally via `safemath`.
- deterministic execution.
- host-testable.

## 8. MVP tests

Required tests:

- read inside bounds,
- read outside bounds,
- write inside bounds,
- write outside bounds,
- write to readonly span,
- memcpy valid,
- memcpy invalid,
- arena alloc valid,
- arena overflow,
- alignment behavior.
