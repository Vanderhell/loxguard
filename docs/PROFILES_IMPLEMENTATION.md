# Profiles - Initial Implementation Defaults

## Implemented profile baseline
Current code behaves as:
- `LOXGUARD_PROFILE_MINIMAL` + checked-memory MVP subset

Included now:
- checked spans (`read_u8`, `write_u8`, `read`, `write`, `memcpy`)
- scratch arena allocator
- event creation for OOB and arena overflow
- basic policy mapping
- in-RAM blackbox ring
- inspectable report

## Proposed compile-time profile macros
```c
#define LOXGUARD_PROFILE_MINIMAL   1
#define LOXGUARD_PROFILE_BLACKBOX  2
#define LOXGUARD_PROFILE_RECOVERY  3
#define LOXGUARD_PROFILE_FULL      4
```

## Planned incremental profile behavior
1. MINIMAL: current baseline (implemented).
2. BLACKBOX: add configurable event depth and export hook.
3. RECOVERY: add callback-based recovery modes.
4. FULL: add adapter integration points and optional serialization.

## Constraints kept
- C99
- no heap by default
- deterministic core
- host-testable first
