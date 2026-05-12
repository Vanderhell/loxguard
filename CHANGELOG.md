# Changelog

All notable changes to this project are documented in this file.

## Unreleased

- Documentation and repository hygiene updates.

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
