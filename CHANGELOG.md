# Changelog

All notable changes to this project are documented in this file.

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

### Companion ecosystem integrations
- `safemath` integration for overflow-safe arithmetic in checked paths (when available).
- `microlog` integration in adapter logging path (when available).
- Partial `microhealth` mapping integration (adapter-level health state mapping).
- Optional host `nvlog` persistence integration:
  - enabled with `LOXGUARD_USE_NVLOG=ON`,
  - host RAM/file backend wiring via nvlog posix backend.

### Build/test verification
- Verified default configuration:
  - `cmake -S . -B build`
  - `cmake --build build --config Debug`
  - `ctest --test-dir build -C Debug --output-on-failure`
- Verified nvlog-enabled configuration:
  - `cmake -S . -B build_nvlog -DLOXGUARD_USE_NVLOG=ON`
  - `cmake --build build_nvlog --config Debug`
  - `ctest --test-dir build_nvlog -C Debug --output-on-failure`
- Verified no-ecosystem default configuration (optional companion modules absent):
  - `cmake -S . -B build_noeco`
  - `cmake --build build_noeco --config Debug`
  - `ctest --test-dir build_noeco -C Debug --output-on-failure`

### Notes
- `ecosystem/` is canonical companion module layout.
- Legacy folder-name fallback `third_party/` remains supported for migration.
- This tag is host-tested MVP, not production embedded backend readiness.
