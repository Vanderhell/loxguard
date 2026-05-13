# Release Notes: v1.0.0

This is the first stable (`v1.0.0`) release of loxguard as a host-tested C99 library.

## Scope of the v1.0.0 stability claim

`v1.0.0` is a stable public contract for:

- Guard Block execution (`loxguard_run(...)`)
- Checked Span/Arena helpers (`lox_span_*`, `lox_arena_*`, and `loxguard_checked.h` macros)
- Blackbox evidence core (`lox_blackbox_t` and event/report types)
- Host diagnostic export/import formats (`loxguard_format.h`) as documented in `docs/API_STABILITY.md`

Out of scope / not verified by this repository:

- production RTOS backend behavior
- production MPU backend behavior
- embedded hardware behavior (ESP32 or other MCU)
- power-loss / restart consistency
- companion-enabled ecosystem builds unless the companion sources are checked in and exercised by CI

## API stability

See `docs/API_STABILITY.md` for:
- what is stable starting at `v1.0.0`
- what is experimental and may change in `1.x`
- deprecation policy and format compatibility policy

## Build and integration

This release focuses on professional library consumption:

- `LOXGUARD_BUILD_TESTS` defaults to `ON` only when loxguard is the top-level project.
- `loxguard::loxguard` is exported for both `add_subdirectory(...)` and install + `find_package(loxguard CONFIG REQUIRED)` usage.
- Consumer smoke tests are included under `tests/consumer/` and executed via CTest.

## Evidence

For what is verified by repository evidence (CI/tests/artifacts) vs. not verified, see `docs/EVIDENCE_MATRIX.md`.
