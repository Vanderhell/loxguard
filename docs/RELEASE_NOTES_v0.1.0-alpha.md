# loxguard v0.1.0-alpha Release Notes

Date: 2026-04-30  
Release type: host-tested MVP alpha

## Included in this release

- Guard Blocks host MVP core.
- Checked span/arena primitives with boundary enforcement.
- Structured event pipeline and policy decisions.
- Blackbox evidence and report generation.
- CSV event export/import and report KV export/import parser paths.
- Companion ecosystem integrations:
  - `safemath` (checked arithmetic path),
  - `microlog` (logging adapter path),
  - partial `microhealth` mapping,
  - optional host `nvlog` persistence (`LOXGUARD_USE_NVLOG=ON`).

## Verified

- Default host build and tests pass.
- nvlog-enabled host build and tests pass.
- Default host build and tests pass even when `ecosystem/` is absent (companion integrations optional).
- Claim-to-test mapping documented in `docs/EVIDENCE_MATRIX.md`.

## Not verified

- Production RTOS backend behavior on real RTOS targets.
- Production MPU backend behavior on real hardware.
- LLVM/compiler plugin instrumentation mode.
- Production embedded flash/EEPROM/FRAM persistence behavior.
- Runtime integrations for `microwdt`, `microtest`, `microbus`, `microres`, `microsh`, `loxdb`.

## Exact build and test commands

Default:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

nvlog-enabled:

```powershell
cmake -S . -B build_nvlog -DLOXGUARD_USE_NVLOG=ON
cmake --build build_nvlog --config Debug
ctest --test-dir build_nvlog -C Debug --output-on-failure
```

No-ecosystem default verification:

```powershell
cmake -S . -B build_noeco
cmake --build build_noeco --config Debug
ctest --test-dir build_noeco -C Debug --output-on-failure
```

## Known limitations

- This release is not a production embedded deployment backend.
- RTOS/MPU behavior is modeled through host/stub paths.
- Optional companion integration behavior depends on local companion module availability.
- `nvlog` persistence verification is host RAM/file oriented only.

## Companion library notes

- Companion modules are part of the same ecosystem (not foreign third-party dependencies).
- Canonical path: `ecosystem/`.
- Legacy folder-name fallback: `third_party/` (migration compatibility only).
- Companion modules are optional and additive to host MVP core.
