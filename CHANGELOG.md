# Changelog

All notable changes to this project are documented in this file.

## Unreleased

- (no entries)

## v1.0.0 (2026-05-13)

### Stabilized (public contract)
- Stable host-tested core API surface defined in `docs/API_STABILITY.md`.
- Host diagnostic export/import formats documented as stable for `1.x` (`docs/API_STABILITY.md`, `docs/FORMAT_EXPORTS.md`).

### Build / integration
- `LOXGUARD_BUILD_TESTS` defaults to `ON` only for top-level builds (avoids building tests/demos in superprojects).
- Ecosystem discovery checks both the loxguard-local and superproject paths (`ecosystem/` and `third_party/`).
- Consumer smoke tests added for:
  - `add_subdirectory(...)` consumption,
  - install + `find_package(loxguard CONFIG REQUIRED)` consumption.

### Documentation
- README and core docs updated to match the `v1.0.0` scope: host-tested core is stable; RTOS/MPU/embedded/power-loss and companion builds remain not verified unless raw repo artifacts exist.

## v0.1.0-alpha (2026-04-30)

### Added
- Guard Blocks host MVP flow (`detect -> classify -> policy -> action -> evidence -> report`).
- Checked primitives:
  - checked spans (`read/write/memcpy` boundary checks),
  - checked arena allocation/overflow handling.
- Guard lifecycle event coverage for success and failure:
  - `BLOCK_ENTERED`, `BLOCK_OK`, `BLOCK_COMPLETED`,
  - failure events including bounds/arena/timeout/memory-fault/unsupported.
- Policy engine decisions and recovery callback hook.
- Blackbox evidence storage with bounded rollover and owned event text.
- Report model with result/action/duration/persisted fields.
- CSV event export/import and report KV export/import parsing utilities.

### Optional integrations (requires external sources)
- `safemath` integration for overflow-safe arithmetic in checked paths (when available).
- `microlog` integration in adapter logging path (when available).
- Partial `microhealth` mapping integration (adapter-level health state mapping).
- Optional host `nvlog` persistence adapter path (`LOXGUARD_USE_NVLOG=ON`, when available).
- Optional `microtimer` / `microwdt` / `microres` adapter paths (when available).

### Verification
- Verified in CI:
  - default build + tests (Windows/Linux/macOS)
  - no-ecosystem build + tests (Windows/Linux/macOS)
  - clang ASan/UBSan build + tests (Ubuntu)
- Companion-enabled builds require companion sources and are not part of the default CI matrix.
