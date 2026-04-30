# Prompt 13 — MVP checked parser demo design

## Task

Design the first strong MVP demo: checked parser Guard Block.

## Required output

Create/update:

```text
docs/MVP_CHECKED_PARSER_DEMO.md
```

## Scenario

```text
packet_parser Guard Block enters
input span len=8
output span len=16
scratch arena len=64
parser intentionally writes output index=17
loxguard detects bounds violation before write
event BLOCK_WRITE_OUT_OF_BOUNDS created
policy action DROP_INPUT
blackbox stores event
report printed
```

## Required output

```text
[loxguard] init
[guard] enter packet_parser
[check] output write out of bounds index=17 len=16
[event] BLOCK_WRITE_OUT_OF_BOUNDS source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser reason=BOUNDS action=DROP_INPUT mode=NORMAL
```

## Required tests

- valid parse path,
- invalid write path,
- invalid write not performed,
- event emitted,
- policy action selected,
- blackbox updated,
- report contains expected data.

## Do not

- Do not require MCU.
- Do not require MPU.
- Do not require plugin.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
