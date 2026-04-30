# Prompt 03 — Failure pipeline design

## Task

Design how Guard Block failures move through the pipeline.

## Required output

Create/update:

```text
docs/FAILURE_PIPELINE.md
```

## Must cover

- detect,
- classify,
- decide,
- act,
- persist,
- inspect.

## Must include example

```text
packet_parser fails
→ BLOCK_ERROR event
→ policy DROP_INPUT
→ blackbox stored
→ report printed
```

## Do not

- Do not write code.
- Do not make this only about watchdogs.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
