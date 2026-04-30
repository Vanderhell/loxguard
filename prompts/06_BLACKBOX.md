# Prompt 06 — Blackbox

## Task

Design local blackbox evidence system.

## Required output

Create/update:

```text
docs/BLACKBOX.md
```

## Must include

- last failed block,
- last event,
- last action,
- system mode,
- recent event ring,
- report format,
- RAM-only MVP,
- optional nvlog/loxdb later.

## MVP report example

```text
last_failed_block=packet_parser
last_event=BLOCK_ERROR
last_action=DROP_INPUT
mode=NORMAL
```

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
