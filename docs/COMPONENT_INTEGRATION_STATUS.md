# Component Integration Status

Date: 2026-04-30
Scope: loxguard companion library integration strategy (host MVP freeze)

Canonical companion layout: `ecosystem/`
Legacy folder-name fallback supported: `third_party/`

| Component | Found | Path | Public header(s) | Build status in this repo | Integration decision | loxguard role |
|---|---|---|---|---|---|---|
| microtimer | YES | `ecosystem/microtimer` | `include/mtimer.h` | Optionally linked when `LOXGUARD_USE_MICROTIMER=ON` | ADAPT (partial) | optional timer manager integration in adapter path (`mtimer_init/mtimer_create/mtimer_tick`) over the existing `lox_adapter_now_ms` contract |
| microlog | YES | `ecosystem/microlog` | `include/mlog.h` | Linked (`ecosystem/microlog/src/mlog.c`) | REUSE/ADAPT | structured event logging from adapter (`lox_adapter_log_event`) |
| nvlog | YES | `ecosystem/nvlog` | `include/nvlog.h`, `backends/nvlog_posix.h` | Optionally linked when `LOXGUARD_USE_NVLOG=ON` | REUSE/ADAPT (host) | host RAM/file persistence adapter for `lox_adapter_persist_event(...)` via `src/adapters/loxguard_nvlog_adapter.c` |
| microhealth | YES | `ecosystem/microhealth` | `include/mhealth.h` | Linked (`ecosystem/microhealth/src/mhealth.c`) | REUSE/ADAPT (partial) | guard outcome to health severity mapping via adapter-managed metric |
| microwdt | YES | `ecosystem/microwdt` | `include/mwdt.h` | Optionally linked when `LOXGUARD_USE_MICROWDT=ON` | REUSE/ADAPT (partial) | optional watchdog/liveness mapping from guard lifecycle events (`ENTERED/OK/COMPLETED/TIMEOUT/ERROR`) to watchdog state model |
| microtest | YES | `ecosystem/microtest` | `include/mtest.h` | Not linked | POSTPONE | test framework migration candidate |
| safemath | YES | `ecosystem/safemath` | `safemath.h` | Header-only included | REUSE | overflow-safe size/index arithmetic in span/arena checks |
| microbus | YES | `ecosystem/microbus` | `include/mbus.h` | Not linked | POSTPONE | event publication bus |
| microres | YES | `ecosystem/microres` | `include/mres.h` | Optionally linked when `LOXGUARD_USE_MICRORES=ON` | REUSE/ADAPT (partial) | optional recovery/circuit-breaker assist path for repeated guard failures |
| microsh | YES | `ecosystem/microsh` | `include/msh.h` | Not linked | POSTPONE | debug/inspection command surface |
| microassert | NO | N/A | expected `include/massert.h` | Not linked | NOT AVAILABLE / PLANNED | controlled assert/panic adapter path |
| panicdump | NO | N/A | expected `include/panicdump.h` | Not linked | NOT AVAILABLE / PLANNED | fault dump adapter path (host-safe mapping only planned) |
| loxdb | YES | `ecosystem/loxdb` | `include/lox.h` | Not linked | POSTPONE | optional future event history backend |

## Notes

- Companion libraries are optional; loxguard core remains host-buildable without them.
- CI does not fetch companion repositories; companion-enabled builds are validated locally when sources exist under `ecosystem/`.
- This pass keeps host MVP freeze discipline: no unrelated feature work.
- Directly integrated in this pass:
  - `microlog` (structured logging adapter path)
  - `safemath` (checked arithmetic reuse in core span/arena)
  - `microhealth` (adapter-level health mapping)
- `nvlog` is integrated for host RAM/file persistence when `LOXGUARD_USE_NVLOG=ON`.
- `microtimer` is integrated as an optional adapter-managed timing manager path when `LOXGUARD_USE_MICROTIMER=ON`.
- `microwdt` is integrated as an optional adapter-managed watchdog/liveness mapping path when `LOXGUARD_USE_MICROWDT=ON`.
- `microres` is integrated as an optional adapter-managed recovery/circuit-breaker assist path when `LOXGUARD_USE_MICRORES=ON`.
- Not integrated yet (intentionally): `microtest`, `microbus`, `microsh`, `loxdb`.
- These postponed ecosystem integrations are **NOT VERIFIED** in runtime behavior for loxguard yet.
- Embedded flash/EEPROM/FRAM persistence behavior through nvlog is **NOT VERIFIED** in loxguard.

## microtimer/microwdt/microres API notes

- `microtimer` APIs used:
  - `mtimer_init`, `mtimer_create`, `mtimer_start`, `mtimer_tick`
- Integration note:
  - `microtimer` expects a clock callback and does not provide a raw system clock by itself.
  - loxguard keeps `lox_adapter_set_time_now(...)` as the deterministic time source contract and layers `microtimer` on top when enabled.

- `microwdt` APIs used:
  - `mwdt_init`, `mwdt_set_timeout_cb`, `mwdt_register`, `mwdt_kick`
- Mapping note:
  - `BLOCK_ENTERED` / `BLOCK_OK` -> watchdog OK kick path
  - `BLOCK_TIMEOUT` -> watchdog non-OK state mapping (late/starved semantics)
  - `BLOCK_ERROR` / bounds / arena / memory-fault -> watchdog non-OK state mapping
  - `WATCHDOG_LATE` / `WATCHDOG_STARVED` are represented in this wave through adapter watchdog state (`lox_adapter_watchdog_state_get`) and are not standalone `lox_event_t` kinds yet.

- `microres` APIs used:
  - `mres_breaker_init`, `mres_breaker_report_success`, `mres_breaker_report_failure`
  - `mres_breaker_state`, `mres_breaker_remaining_ms`, `mres_breaker_reset`
- Mapping note:
  - repeated guard failures are reported into `microres` breaker state
  - open breaker state blocks guarded execution attempt and emits `BLOCK_ERROR` with `reason=BREAKER_OPEN`
  - successful runs report success to breaker and can move state toward closed/healthy

- `microassert`:
  - optional build switch exists: `LOXGUARD_USE_MICROASSERT`
  - current workspace does not contain the companion module, so runtime integration is not verified

- `panicdump`:
  - optional build switch exists: `LOXGUARD_USE_PANICDUMP`
  - current workspace does not contain the companion module, so runtime integration is not verified
