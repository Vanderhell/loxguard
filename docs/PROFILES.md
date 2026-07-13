# Profiles

`LOXGUARD_PROFILE_*` values are descriptive bundle labels only. They do not gate
stable headers or source files, and they do not convert stub/demo paths into
production support claims.

## LOXGUARD_PROFILE_MINIMAL

Stable core only:

- Guard Blocks,
- event schema,
- basic policy,
- RAM blackbox.

Use when you want the smallest host-tested surface.

## LOXGUARD_PROFILE_BLACKBOX

Adds:

- recent event ring,
- report function,
- optional logging/persistence adapters when companion sources are present.

This is still a host-tested build profile, not a production backend claim.

## LOXGUARD_PROFILE_RECOVERY

Adds:

- recovery callbacks,
- degraded mode,
- safe mode,
- retry/circuit-breaker integration.

These hooks are experimental and notification-oriented. They do not perform
automatic cleanup or resource release.

## LOXGUARD_PROFILE_FULL

Adds:

- event bus,
- debug shell,
- config,
- serialization/export,
- persistent storage bridge.

This bundle remains experimental until the companion sources are present and
the corresponding host tests exercise them.

## LOXGUARD_PROFILE_RTOS

Adds:

- task/job supervision,
- task watchdog,
- stack high-water integration.

Current RTOS-facing paths are stub/synthetic on the host unless companion
sources and target evidence are provided. Do not treat the `FREERTOS_STUB`
label as production RTOS support.

## LOXGUARD_PROFILE_MPU

Adds:

- hardware-contained Guard Blocks,
- MemManage/fault event mapping,
- module quarantine.

Current MPU-facing paths are stub/synthetic on the host unless companion
sources and raw hardware evidence are provided. Do not treat the
`CORTEXM_STUB` label as production MPU support.

## Important

MPU is optional.

The product must be useful before MPU exists.
