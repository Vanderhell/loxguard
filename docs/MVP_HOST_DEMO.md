# MVP Host Demo

## Goal

Prove the Guard Block product model without MCU-specific code.

## Scenario

```text
packet_parser Guard Block enters
parser returns error or simulated timeout
loxguard creates event
policy chooses action
blackbox stores evidence
report prints result
```

## Expected output

```text
[loxguard] init
[guard] enter packet_parser
[guard] packet_parser failed: ERROR
[event] BLOCK_ERROR source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser action=DROP_INPUT mode=NORMAL
```

## Required tests

- block enters,
- block OK result,
- block ERROR result,
- timeout simulation,
- event created,
- policy selected,
- blackbox stored,
- report contains last failed block.

## Non-goals

- no MPU,
- no RTOS,
- no flash persistence,
- no dynamic loading,
- no signed modules,
- no OTA.

## v4 Checked Parser MVP

The preferred MVP demo is now a checked parser demo.

Scenario:

```text
packet_parser receives:
  input span len=8
  output span len=16
  scratch arena len=64

parser intentionally writes output index=17
lox_span_write_u8 detects bounds violation
write is not performed
loxguard emits BLOCK_WRITE_OUT_OF_BOUNDS
policy selects DROP_INPUT
blackbox stores evidence
report prints last_failed_block=packet_parser reason=BOUNDS
```

Expected output:

```text
[loxguard] init
[guard] enter packet_parser
[check] output write out of bounds index=17 len=16
[event] BLOCK_WRITE_OUT_OF_BOUNDS source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser reason=BOUNDS action=DROP_INPUT mode=NORMAL
```
