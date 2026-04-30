# Component Integration Status

Date: 2026-04-30
Scope: loxguard companion library integration strategy (host MVP freeze)

Canonical companion layout: `ecosystem/`
Legacy folder-name fallback supported: `third_party/`

| Component | Found | Path | Public header(s) | Build status in this repo | Integration decision | loxguard role |
|---|---|---|---|---|---|---|
| microtimer | YES | `ecosystem/microtimer` | `include/mtimer.h` | Not linked directly | ADAPT (partial) | unified time contract for duration/ticks via adapter clock; microtimer itself is timer scheduler, not raw clock source |
| microlog | YES | `ecosystem/microlog` | `include/mlog.h` | Linked (`ecosystem/microlog/src/mlog.c`) | REUSE/ADAPT | structured event logging from adapter (`lox_adapter_log_event`) |
| nvlog | YES | `ecosystem/nvlog` | `include/nvlog.h`, `backends/nvlog_posix.h` | Optionally linked when `LOXGUARD_USE_NVLOG=ON` | REUSE/ADAPT (host) | host RAM/file persistence adapter for `lox_adapter_persist_event(...)` via `src/adapters/loxguard_nvlog_adapter.c` |
| microhealth | YES | `ecosystem/microhealth` | `include/mhealth.h` | Linked (`ecosystem/microhealth/src/mhealth.c`) | REUSE/ADAPT (partial) | guard outcome to health severity mapping via adapter-managed metric |
| microwdt | YES | `ecosystem/microwdt` | `include/mwdt.h` | Not linked | POSTPONE | liveness/timeout watchdog integration |
| microtest | YES | `ecosystem/microtest` | `include/mtest.h` | Not linked | POSTPONE | test framework migration candidate |
| safemath | YES | `ecosystem/safemath` | `safemath.h` | Header-only included | REUSE | overflow-safe size/index arithmetic in span/arena checks |
| microbus | YES | `ecosystem/microbus` | `include/mbus.h` | Not linked | POSTPONE | event publication bus |
| microres | YES | `ecosystem/microres` | `include/mres.h` | Not linked | POSTPONE | advanced recovery/retry policy engine |
| microsh | YES | `ecosystem/microsh` | `include/msh.h` | Not linked | POSTPONE | debug/inspection command surface |
| loxdb | YES | `ecosystem/loxdb` | `include/lox.h` | Not linked | POSTPONE | optional future event history backend |

## Notes

- Companion libraries were verified via `git ls-remote` and cloned into the local ecosystem folder.
- This pass keeps host MVP freeze discipline: no unrelated feature work.
- Directly integrated in this pass:
  - `microlog` (structured logging adapter path)
  - `safemath` (checked arithmetic reuse in core span/arena)
  - `microhealth` (adapter-level health mapping)
- `nvlog` is integrated for host RAM/file persistence when `LOXGUARD_USE_NVLOG=ON`.
- Not integrated yet (intentionally): `microwdt`, `microtest`, `microbus`, `microres`, `microsh`, `loxdb`.
- These postponed ecosystem integrations are **NOT VERIFIED** in runtime behavior for loxguard yet.
- Embedded flash/EEPROM/FRAM persistence behavior through nvlog is **NOT VERIFIED** in loxguard.
