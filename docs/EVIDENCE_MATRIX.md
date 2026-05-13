# Evidence Matrix

Date: 2026-05-13  
Scope: repository-verifiable behavior only (code, tests, CI logs, and checked-in raw artifacts)

This file lists what is verified by repository evidence and what is explicitly **NOT VERIFIED**.

## Raw artifacts

- Host logs: `artifacts/evidence/host/` (if present)
- Hardware logs: `artifacts/evidence/esp32/` (placeholders only unless raw logs are added)

## CI scope (GitHub Actions)

Workflow: `.github/workflows/ci.yml`

CI is intended to verify:
- `default` build + tests
- `no-ecosystem` build + tests (CI deletes `ecosystem/`)
- clang ASan/UBSan build + tests (Ubuntu)
- consumer smoke tests (via CTest): `add_subdirectory` and `install+find_package`

CI does not verify by default:
- companion-enabled configurations that require external sources under `ecosystem/` / `third_party/`

## Claims and status

| Claim | Evidence in repo | Status |
|---|---|---|
| Guard Block wrapper executes and reports result/action | Unit tests under `tests/` | VERIFIED |
| Checked span helpers reject out-of-bounds operations | Unit tests under `tests/` | VERIFIED |
| Arena helpers reject overflow / invalid state | Unit tests under `tests/` | VERIFIED |
| Blackbox stores structured events with bounded rollover | Unit tests under `tests/` | VERIFIED |
| Export/import format behavior (event/report lines) | Unit tests under `tests/` + `docs/API_STABILITY.md` + `docs/FORMAT_EXPORTS.md` | VERIFIED |
| Demo “panic/fault” host paths produce structured events | Unit tests under `tests/` | VERIFIED |
| Library consumption via `add_subdirectory(...)` | Consumer smoke test projects under `tests/consumer/` | VERIFIED (host) |
| Install + `find_package(loxguard CONFIG REQUIRED)` works | Consumer smoke test projects under `tests/consumer/` | VERIFIED (host) |
| Companion integrations are present and exercised | `ecosystem/` is not vendored; CI does not build companion profiles | NOT VERIFIED |
| RTOS/MPU production containment behavior | Stub/demo interfaces only | NOT VERIFIED |
| Embedded hardware behavior (ESP32 or other MCU) | No raw MCU logs are checked in under `artifacts/evidence/esp32/` | NOT VERIFIED |
| Power-loss / restart consistency | No raw artifacts are checked in under `artifacts/evidence/esp32/` | NOT VERIFIED |

## Notes

- “VERIFIED (host)” means: verified by host CI/tests and/or checked-in host artifacts, not by embedded hardware evidence.
- Do not describe embedded behavior as verified unless raw artifacts exist under `artifacts/evidence/` and are referenced here.
