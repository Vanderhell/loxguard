# Policy Engine

## Purpose

The policy engine turns failures into actions.

Input:

```text
event + source/block config + history + criticality + current mode
```

Output:

```text
one or more actions
```

## Actions

```text
NONE
LOG
WARN
DROP_INPUT
RECOVER_BLOCK
RECOVER_MODULE
RESTART_MODULE
DISABLE_MODULE
QUARANTINE_MODULE
ENTER_DEGRADED_MODE
ENTER_SAFE_MODE
REBOOT
ROLLBACK_UPDATE
STORE_BLACKBOX
PUBLISH_EVENT
```

## MVP policy examples

```text
IF block returns ERROR
AND block is optional
THEN DROP_INPUT + STORE_BLACKBOX
```

```text
IF block TIMEOUT count < max_failures
THEN RECOVER_BLOCK + STORE_BLACKBOX
```

```text
IF block TIMEOUT count >= max_failures
THEN DISABLE_MODULE + ENTER_DEGRADED_MODE + STORE_BLACKBOX
```

```text
IF critical source faults
THEN ENTER_SAFE_MODE + STORE_BLACKBOX
```

## Constraints

- deterministic,
- bounded,
- static by default,
- no dynamic scripting,
- no heap by default,
- host-testable.

## v4 additions for Checked Guard Blocks

Recommended default policies:

```text
BLOCK_BOUNDS_VIOLATION in optional parser
→ DROP_INPUT + STORE_BLACKBOX

BLOCK_ARENA_OVERFLOW in optional module
→ RECOVER_BLOCK + STORE_BLACKBOX

BLOCK_CONTRACT_VIOLATION repeated >= max_failures
→ DISABLE_MODULE + ENTER_DEGRADED_MODE

BLOCK_FORBIDDEN_CALL
→ QUARANTINE_MODULE + STORE_BLACKBOX
```

Policy must record whether the unsafe operation was prevented or whether the system only detected a failure after it happened.
