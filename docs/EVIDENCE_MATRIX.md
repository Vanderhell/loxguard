# Evidence Matrix

Date: 2026-07-14
Scope: repository-verifiable behavior only (source, tests, CI definitions, and checked-in raw artifacts)

Status legend:

- VERIFIED: the repo contains direct source/test evidence or checked-in raw artifacts for the claim.
- VERIFIED WITH DEFINED LIMITS: the claim is supported, but only within the stated host/CI constraints.
- NOT VERIFIED: the repository does not support the claim.
- INCOMPLETE: the gate or feature exists, but the repo does not yet include the raw evidence needed to promote the claim.

## Raw artifacts

- Host logs: `artifacts/evidence/host/`
- Hardware logs: `artifacts/evidence/esp32/`

## Claims and status

| Claim | Evidence in repo | Status |
|---|---|---|
| Guard Block wrapper executes and reports result/action | Unit tests under `tests/`, host logs in `artifacts/evidence/host/` | VERIFIED |
| Checked span helpers reject out-of-bounds operations | Unit tests under `tests/` | VERIFIED |
| Arena helpers reject overflow / invalid state | Unit tests under `tests/` | VERIFIED |
| Blackbox stores structured events with bounded rollover | Unit tests under `tests/` | VERIFIED |
| Export/import format behavior for event and report lines | Unit tests under `tests/`, `docs/API_STABILITY.md`, `docs/FORMAT_EXPORTS.md` | VERIFIED |
| Demo "panic/fault" host paths produce structured events | Unit tests under `tests/` | VERIFIED |
| Host build/test verification on Windows | `artifacts/evidence/host/windows_build_verify_notes.txt`, `artifacts/evidence/host/windows_build_verify_ctest.txt` | VERIFIED |
| Host build/test verification on WSL GCC and Clang | `artifacts/evidence/host/wsl_ubuntu24_gcc_build_gcc.txt`, `artifacts/evidence/host/wsl_ubuntu24_clang_build_clang.txt` | VERIFIED WITH DEFINED LIMITS |
| GCC strict C99 Debug/Release builds | Existing verification report | VERIFIED WITH DEFINED LIMITS |
| Clang strict C99 Debug/Release builds | Existing verification report | VERIFIED WITH DEFINED LIMITS |
| Package install | Existing verification report | VERIFIED WITH DEFINED LIMITS |
| Version/tag alignment for `v1.0.0` | `v1.0.0` tag, `CMakeLists.txt`, `include/loxguard.h`, `CHANGELOG.md`, `docs/RELEASE_NOTES_v1.0.0.md`, `tools/version_consistency_check.cmake` | VERIFIED |
| Companions are checked in under `ecosystem/` | Checked-in source tree under `ecosystem/` | VERIFIED |
| Companion-enabled configurations are exercised by default CI | `.github/workflows/ci.yml` does not include a companion-source build matrix | NOT VERIFIED |
| Library consumption via `add_subdirectory(...)` | `tests/consumer/`, `tools/consumer_smoke_test.cmake`, `.github/workflows/ci.yml`, existing verification report | VERIFIED WITH DEFINED LIMITS |
| Install + `find_package(loxguard CONFIG REQUIRED)` works | `tests/consumer/`, `tools/consumer_smoke_test.cmake`, `.github/workflows/ci.yml`, existing verification report | VERIFIED WITH DEFINED LIMITS |
| Compile-fail contracts gate | `tools/compile_fail_contracts_check.cmake`, `.github/workflows/ci.yml`, existing verification report | VERIFIED WITH DEFINED LIMITS |
| ARM compile-only gate | `tools/arm_cortexm_compile_only.cmake`, `.github/workflows/ci.yml` | INCOMPLETE |
| Embedded hardware behavior (ESP32 or other MCU) | `artifacts/evidence/esp32/` contains placeholders only | NOT VERIFIED |
| Power-loss / restart consistency | No raw target evidence is checked in | NOT VERIFIED |
| RTOS/MPU production containment behavior | Stub/demo interfaces only | NOT VERIFIED |

## Version and release review

- `v1.0.0` is the current tagged release and matches `project(loxguard VERSION 1.0.0)` plus the public header version macros.
- The release gate checks tag/version consistency through `tools/version_consistency_check.cmake`.
- This evidence set does not justify a recommended next version.
