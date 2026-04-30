# v4 alias/update

This prompt supersedes or updates `04_EVENT_SCHEMA.md` in the v4 order.

# Prompt 04 — Event schema

## Task

Design the unified event schema for Guard Blocks and modules.

## Required output

Create/update:

```text
docs/EVENT_SCHEMA.md
```

Optionally draft:

```text
include/loxguard_event.h
```

only when code skeleton is allowed.

## Must support

- block entered,
- block OK,
- block error,
- block timeout,
- block panic,
- block memory fault,
- module health events,
- recovery events,
- blackbox events.

## Constraints

- fixed-size records,
- C99,
- no heap,
- persistent-friendly,
- no raw pointers in persisted events.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
