# Guard Memory Contracts

## 1. Purpose

A memory contract defines what a Checked Guard Block is allowed to access.

Without a contract, loxguard cannot know which pointer access is valid.

## 2. Contract elements

A contract may include:

```text
read spans
write spans
scratch arena
max stack
max recursion
allowed functions
capabilities
```

## 3. Span model

A span is a bounded memory region.

```c
typedef struct lox_span {
    uint8_t *base;
    size_t len;
    uint32_t flags;
} lox_span_t;
```

Flags may include:

```text
READ
WRITE
CONST
VOLATILE
```

## 4. Arena model

An arena is a bounded scratch memory region.

```c
typedef struct lox_arena {
    uint8_t *base;
    size_t size;
    size_t used;
} lox_arena_t;
```

## 5. Contract example

```c
typedef struct parser_guard_ctx {
    lox_span_t input;
    lox_span_t output;
    lox_arena_t scratch;
} parser_guard_ctx_t;
```

The parser should operate through checked accessors:

```c
lox_span_read_u8(&ctx->input, i, &b);
lox_span_write_u8(&ctx->output, j, b);
lox_arena_alloc(&ctx->scratch, size, align);
```

## 6. Violation types

Possible contract violations:

```text
READ_OUT_OF_BOUNDS
WRITE_OUT_OF_BOUNDS
ARENA_OVERFLOW
INVALID_ALIGNMENT
NULL_SPAN
WRITE_TO_READONLY_SPAN
COPY_OVERFLOW
SIZE_OVERFLOW
RECURSION_LIMIT
STACK_BUDGET_EXCEEDED
FORBIDDEN_CALL
```

## 7. Event mapping

Contract violations must create events:

```text
BLOCK_BOUNDS_VIOLATION
BLOCK_ARENA_VIOLATION
BLOCK_CONTRACT_VIOLATION
```

## 8. Important rule

A memory contract is only effective when the guarded code follows the checked API or when compiler instrumentation is enabled.
