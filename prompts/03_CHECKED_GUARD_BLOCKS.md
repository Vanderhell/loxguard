# Prompt 03 — Checked Guard Blocks

## Task

Design Checked Guard Blocks as the Level 2 loxguard feature.

## Required output

Create/update:

```text
docs/CHECKED_GUARD_BLOCKS.md
```

## Required content

- definition,
- why this exists,
- what it catches,
- what it does not catch,
- relation to Soft Guard Blocks,
- relation to MPU Guard Blocks,
- MVP scope,
- expected checked parser demo.

## Required positioning

```text
Checked Guard Blocks detect memory contract violations when guarded code uses loxguard's checked span/arena API.
```

## Do not

- Do not claim arbitrary C memory safety.
- Do not require LLVM/plugin.
- Do not implement code yet.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
