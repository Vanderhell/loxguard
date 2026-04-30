# Event Schema

## Purpose

All Guard Block and module failures become a common event.

## Initial C shape

```c
typedef struct loxguard_event {
    uint32_t tick;
    uint16_t source_id;
    uint16_t event_type;

    uint8_t severity;
    uint8_t state_before;
    uint8_t state_after;
    uint8_t action_taken;

    uint32_t detail0;
    uint32_t detail1;
    uint32_t detail2;
    uint32_t detail3;
} loxguard_event_t;
```

## Event types

```text
BLOCK_ENTERED
BLOCK_OK
BLOCK_ERROR
BLOCK_TIMEOUT
BLOCK_PANIC
BLOCK_FAULT
BLOCK_MEMORY_FAULT
MODULE_REGISTERED
HEARTBEAT_OK
HEARTBEAT_LATE
WATCHDOG_STARVED
RECOVERY_STARTED
RECOVERY_OK
RECOVERY_FAILED
MODULE_DISABLED
MODULE_QUARANTINED
DEGRADED_MODE_ENTERED
SAFE_MODE_ENTERED
PANIC_REPORTED
BOOT_LOOP_DETECTED
BLACKBOX_STORED
```

## Severity

```text
INFO
WARN
FAULT
CRITICAL
FATAL
```

## Rules

- Events describe facts.
- Policies describe decisions.
- Persistent events must not contain raw pointers.
- Event format must be fixed-size and ring-buffer friendly.
- No heap required.

## v4 additions for Checked Guard Blocks

Additional event types:

```text
BLOCK_BOUNDS_VIOLATION
BLOCK_READ_OUT_OF_BOUNDS
BLOCK_WRITE_OUT_OF_BOUNDS
BLOCK_ARENA_OVERFLOW
BLOCK_CONTRACT_VIOLATION
BLOCK_FORBIDDEN_CALL
BLOCK_STACK_BUDGET_EXCEEDED
BLOCK_RECURSION_LIMIT
```

These events must be generated before the unsafe operation is executed whenever possible.

Example:

```text
output span len=16
write index=17
→ BLOCK_WRITE_OUT_OF_BOUNDS
→ write is not performed
```
