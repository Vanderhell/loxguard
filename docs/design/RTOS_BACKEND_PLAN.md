# RTOS Backend Plan

## Purpose

RTOS backend maps Guard Blocks or guarded modules to RTOS primitives.

## Possible features

- task watchdog,
- task health state,
- task restart if supported,
- stack high-water monitoring,
- queue timeout monitoring,
- event bridge to loxguard,
- blackbox record.

## FreeRTOS candidate mapping

```text
Guarded module → task
heartbeat → task kick/check-in
timeout → watchdog event
stack high-water → diagnostic event
recovery → task restart/reinit if safe
```

## Limitations

RTOS backend does not automatically provide memory isolation.

Memory containment still requires MPU/MMU/PMP or similar hardware.
