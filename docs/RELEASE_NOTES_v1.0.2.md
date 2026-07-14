# Release Notes: v1.0.2

`v1.0.2` is a patch release over `v1.0.1`.

## What changed

- Release/tag automation was corrected so the release workflow uses the full semantic tag value.
- Project version and public version macros were bumped to `1.0.2`.
- Release-facing documentation was refreshed for the current tag.

## What did not change

- No public API surface change relative to `v1.0.1`.
- No new feature behavior was introduced.
- The verified scope remains host-oriented; embedded hardware, power-loss, and RTOS/MPU behavior remain outside repository verification.

## Version alignment

- `CMakeLists.txt` matches the `v1.0.2` tag.
- `include/loxguard.h` matches the `v1.0.2` tag.
- `tools/version_consistency_check.cmake` remains the release gate used to enforce tag/version alignment.

## Evidence

See `docs/EVIDENCE_MATRIX.md` for the repository-verifiable scope and current limitations.
