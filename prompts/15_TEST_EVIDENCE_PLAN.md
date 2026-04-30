# v4 alias/update

This prompt supersedes or updates `12_TEST_EVIDENCE_PLAN.md` in the v4 order.

# Prompt 12 — Test evidence plan

## Task

Create test/evidence plan.

## Required output

Create/update:

```text
docs/TEST_EVIDENCE_PLAN.md
```

## Required claim mapping

Map each claim to test/demo evidence.

Claims:

- Guard Block can run OK code,
- Guard Block can report ERROR,
- timeout can be represented,
- event is created,
- policy selects action,
- blackbox stores event,
- report is inspectable,
- no heap by default if claimed.

## Do not

- Do not fake test results.
- Do not claim hardware/MPU behavior.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
