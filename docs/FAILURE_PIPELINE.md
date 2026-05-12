# Failure Pipeline

## Pipeline

```text
detect -> classify -> decide -> act -> persist -> inspect
```

## 1. Detect

Sources:

- Guard Block returned error,
- Guard Block timeout,
- panic/assert,
- watchdog late/starved,
- heartbeat missed,
- boot-loop detected,
- repeated reset,
- recovery failure,
- storage failure,
- communication failure,
- MPU fault where supported.

## 2. Classify

Every failure becomes `loxguard_event_t`.

Classification includes:

- source name/id,
- event type,
- severity,
- state transition,
- criticality,
- count/history,
- diagnostic details.

## 3. Decide

Policy engine maps event + history to action.

Example:

```text
packet_parser ERROR once
→ DROP_INPUT + LOG

comms STARVED 3 times
→ DISABLE_MODULE + DEGRADED_MODE

critical control FAULT
→ SAFE_MODE

application module MEMORY_FAULT
-> QUARANTINE + BLACKBOX
```

## 4. Act

Actions:

- log,
- drop input,
- recover block,
- recover module,
- restart task,
- disable module,
- quarantine module,
- degraded mode,
- safe mode,
- deliberate reboot,
- rollback update later.

## 5. Persist

Backends:

- RAM blackbox,
- nvlog,
- microlog,
- microcbor export,
- optional loxdb,
- UART report.

## 6. Inspect

Inspection:

- blackbox report,
- debug shell,
- UART dump,
- exported records,
- loxdb query later.

## Example output

```text
[guard] enter packet_parser
[guard] packet_parser failed: ERROR
[event] BLOCK_ERROR source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser action=DROP_INPUT
```
