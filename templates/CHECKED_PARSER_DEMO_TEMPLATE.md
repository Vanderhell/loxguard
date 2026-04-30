# Checked Parser Demo Template

## Scenario

```text
input span len:
output span len:
scratch arena len:
intentional invalid operation:
```

## Expected output

```text
[loxguard] init
[guard] enter packet_parser
[check] output write out of bounds index=17 len=16
[event] BLOCK_WRITE_OUT_OF_BOUNDS source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser reason=BOUNDS action=DROP_INPUT mode=NORMAL
```

## Evidence

```text
Command:
Result:
Output:
```

## Verified

- [ ] invalid write was detected
- [ ] invalid write was not performed
- [ ] event was created
- [ ] policy action selected
- [ ] blackbox updated
- [ ] report printed

## Not verified

- [ ] MPU containment
- [ ] compiler instrumentation
- [ ] hardware behavior
