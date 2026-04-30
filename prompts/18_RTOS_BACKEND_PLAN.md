# v4 alias/update

This prompt supersedes or updates `14_RTOS_BACKEND_PLAN.md` in the v4 order.

# Prompt 14 — RTOS backend plan

## Task

Plan optional RTOS backend.

Do not implement it.

## Required output

Create/update:

```text
docs/RTOS_BACKEND_PLAN.md
```

## Must cover

- task-based Guard Blocks,
- task watchdog,
- stack high-water events,
- queue timeout events,
- recovery actions,
- limitations.

## Important

RTOS backend does not automatically provide memory isolation.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
