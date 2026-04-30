# Prompt 02 — Guard Block API design

## Task

Design the first Guard Block and Checked Guard Block API.

## Required output

Create/update:

```text
docs/GUARD_BLOCKS.md
docs/CHECKED_GUARD_BLOCKS.md
```

Optionally draft:

```text
include/loxguard.h
```

only if implementation planning is allowed.

## Required API direction

Function-based first:

```c
typedef int (*loxguard_fn_t)(void *ctx);

int loxguard_run(const loxguard_block_cfg_t *cfg,
                 loxguard_fn_t fn,
                 void *ctx);
```

Checked memory API is separate:

```c
lox_span_t
lox_arena_t
lox_span_read_u8()
lox_span_write_u8()
lox_arena_alloc()
```

## Must define

- block config,
- result codes,
- criticality,
- timeout behavior,
- failure count,
- backend capability flags,
- soft vs checked vs instrumented vs RTOS vs MPU behavior.

## Do not

- Do not start with macro-only syntax.
- Do not claim all memory bugs are catchable.
- Do not implement backend code yet.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
