# Prompt 10 — MVP host demo design

## Task

Design first host-testable MVP demo.

## Required output

Create/update:

```text
docs/MVP_HOST_DEMO.md
```

## Scenario

```text
packet_parser Guard Block enters
parser returns ERROR
loxguard creates BLOCK_ERROR
policy chooses DROP_INPUT
blackbox stores evidence
report prints last_failed_block=packet_parser
```

Also include simulated timeout path.

## Required tests

- OK block,
- ERROR block,
- TIMEOUT simulation,
- event created,
- policy action,
- blackbox report.

## Do not

- Do not require MCU.
- Do not require MPU.
- Do not require RTOS.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
