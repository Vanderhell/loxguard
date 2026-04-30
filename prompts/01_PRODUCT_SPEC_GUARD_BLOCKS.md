# Prompt 01 — Product spec: Guard Blocks + Checked Guard Blocks

## Task

Create/refine product documentation where Guard Blocks and Checked Guard Blocks are the primary model.

## Required output

Create/update:

```text
docs/LOXGUARD_UNIFIED_PRODUCT_SPEC.md
```

## Must include

- loxguard definition,
- Guard Block concept,
- Checked Guard Block concept,
- failure pipeline underneath,
- what loxguard owns,
- what existing libraries may provide,
- soft/checked/instrumented/RTOS/MPU Guard Block levels,
- MVP checked parser demo,
- non-goals and limitations.

## Preserve this identity

```text
loxguard gives embedded C developers Guard Blocks and Checked Guard Blocks: supervised failure boundaries around risky firmware code.
```

## Preserve this slogan

```text
Wrap risky C code. Check its memory. Recover with evidence.
```

## Do not

- Do not present loxguard as only watchdog/logging.
- Do not present loxguard as MPU-only.
- Do not make compiler plugin the MVP.
- Do not implement code.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
