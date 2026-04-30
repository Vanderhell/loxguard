# Prompt 05 — Safe span and arena API

## Task

Design the MVP checked memory API.

## Required output

Create/update:

```text
docs/SAFE_SPAN_ARENA_API.md
```

Optionally draft headers only if implementation planning is explicitly allowed:

```text
include/loxguard_span.h
include/loxguard_arena.h
```

## Required API

- `lox_span_t`,
- `lox_arena_t`,
- readonly/writable span constructors,
- checked read,
- checked write,
- checked memcpy,
- arena init,
- arena alloc,
- arena reset,
- error codes.

## Required tests to define

- valid read,
- invalid read,
- valid write,
- invalid write,
- readonly write rejection,
- valid memcpy,
- invalid memcpy,
- arena alloc,
- arena overflow.

## Do not

- Do not use heap.
- Do not require compiler plugin.
- Do not require MPU.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
