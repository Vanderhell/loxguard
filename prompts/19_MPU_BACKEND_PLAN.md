# v4 alias/update

This prompt supersedes or updates `15_MPU_BACKEND_PLAN.md` in the v4 order.

# Prompt 15 — MPU backend plan

## Task

Plan optional MPU backend.

Do not implement it.

## Required output

Create/update:

```text
docs/MPU_BACKEND_PLAN.md
```

## Must cover

- Cortex-M4 first,
- MemManage fault,
- BLOCK_MEMORY_FAULT event,
- policy integration,
- blackbox evidence,
- hardware validation requirement,
- DMA limitation,
- unsupported targets.

## Important

MPU backend is optional.

loxguard-core must be useful without MPU.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
