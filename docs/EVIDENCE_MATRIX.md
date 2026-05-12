# Evidence Matrix

Date: 2026-05-12  
Scope: repository-verifiable behavior only (builds, tests, and checked-in raw artifacts)

This file lists what is verified by code/tests and what has raw evidence checked into this repository.

## Raw artifacts

- Host logs: `artifacts/evidence/host/`
- Hardware logs: `artifacts/evidence/esp32/` (placeholders only unless raw logs are added)

## CI scope (GitHub Actions)

Workflow: `.github/workflows/ci.yml`

Verified in CI:
- `default` build + tests (Windows, Linux, macOS)
- `no-ecosystem` build + tests (Windows, Linux, macOS)
- clang ASan/UBSan build + tests (Ubuntu)

Not verified in CI:
- companion-enabled configurations that require external sources under `ecosystem/` / `third_party/`

## Claims and status

| Claim | Evidence in repo | Status |
|---|---|---|
| Guard Block wrapper executes and reports result/action | Unit tests under `tests/` (built by CI) | VERIFIED |
| Checked span helpers reject out-of-bounds operations | Unit tests under `tests/` (built by CI) | VERIFIED |
| Arena helpers reject overflow / invalid state | Unit tests under `tests/` (built by CI) | VERIFIED |
| Blackbox stores structured events with bounded rollover | Unit tests under `tests/` (built by CI) | VERIFIED |
| CSV/report format parse/export behavior | `docs/FORMAT_EXPORTS.md` + unit tests under `tests/` | VERIFIED |
| “panic/fault” demo paths produce structured events (host) | Unit tests under `tests/` | VERIFIED |
| Companion integrations are present and exercised | `ecosystem/` is not vendored; CI does not build companion profiles | NOT VERIFIED |
| RTOS/MPU production containment behavior | Stub/demo interfaces only in `v0.1.0-alpha` | NOT VERIFIED |
| Embedded hardware behavior (ESP32 or other MCU) | No raw MCU logs are checked in under `artifacts/evidence/esp32/` | NOT VERIFIED |
| Power-loss / restart consistency | No raw artifacts are checked in under `artifacts/evidence/esp32/` | NOT VERIFIED |

## Host evidence currently checked in

The repository includes the following host logs under `artifacts/evidence/host/`:
- Windows build/test logs
- WSL Ubuntu build logs (gcc and clang)

Refer to the filenames in that directory for the exact runs.
