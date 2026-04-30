# v4 alias/update

This prompt supersedes or updates `16_REVIEW_AND_CONSOLIDATE.md` in the v4 order.

# Prompt 16 — Review and consolidate

## Task

Review all docs/prompts for internal consistency.

## Required checks

- Guard Blocks are first-class.
- Failure pipeline is underneath Guard Blocks.
- Existing libraries are accelerators, not boundaries.
- MPU is optional.
- RTOS is optional.
- Host MVP is first.
- Claims are honest.
- No conflicting old positioning remains.

## Required output

Create/update:

```text
docs/CONSOLIDATION_REVIEW.md
```

## Do not

- Do not implement code.
- Do not remove useful content without noting why.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
