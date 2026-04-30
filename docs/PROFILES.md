# Profiles

## LOXGUARD_PROFILE_MINIMAL

Includes:

- Guard Blocks,
- event schema,
- basic policy,
- RAM blackbox.

Provides:

- controlled block failures,
- basic evidence.

## LOXGUARD_PROFILE_BLACKBOX

Adds:

- recent event ring,
- report function,
- optional nvlog/microlog sink.

## LOXGUARD_PROFILE_RECOVERY

Adds:

- recovery callbacks,
- degraded mode,
- safe mode,
- retry/circuit-breaker integration.

## LOXGUARD_PROFILE_FULL

Adds:

- event bus,
- debug shell,
- config,
- serialization/export,
- persistent storage bridge.

## LOXGUARD_PROFILE_RTOS

Adds:

- task/job supervision,
- task watchdog,
- stack high-water integration.

## LOXGUARD_PROFILE_MPU

Adds:

- hardware-contained Guard Blocks,
- MemManage/fault event mapping,
- module quarantine.

## Important

MPU is optional.

The product must be useful before MPU exists.
