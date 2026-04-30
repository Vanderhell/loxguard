# v4 alias/update

This prompt supersedes or updates `11_MVP_IMPLEMENTATION_PLAN.md` in the v4 order.

# Prompt 11 — MVP implementation plan

## Task

Plan implementation of host MVP.

## Required output

Create/update:

```text
docs/MVP_IMPLEMENTATION_PLAN.md
```

## Proposed files

```text
include/loxguard.h
include/loxguard_event.h
include/loxguard_policy.h
include/loxguard_blackbox.h

src/loxguard_core.c
src/loxguard_event.c
src/loxguard_policy.c
src/loxguard_blackbox.c

examples/host_guard_block_demo/
tests/
```

## Must include

- public API,
- data structures,
- event flow,
- policy flow,
- blackbox flow,
- test plan,
- build command,
- demo output.

## Do not

- Do not implement code unless explicitly asked.
- Do not add MPU/RTOS code.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
