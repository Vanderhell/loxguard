# Release Notes: v1.0.1

`v1.0.1` is a patch release over `v1.0.0`.

## What changed

- Project version and public version macros were bumped to `1.0.1`.
- Release-facing documentation was refreshed for the current tag.
- README badges and project links were added for easier navigation.
- Project-owned cppcheck warnings were removed without changing runtime behavior.

## What did not change

- No public API surface change relative to `v1.0.0`.
- No new feature behavior was introduced.
- The verified scope remains host-oriented; embedded hardware, power-loss, and RTOS/MPU behavior remain outside repository verification.

## Version alignment

- `CMakeLists.txt` matches the `v1.0.1` tag.
- `include/loxguard.h` matches the `v1.0.1` tag.
- `tools/version_consistency_check.cmake` remains the release gate used to enforce tag/version alignment.

## Evidence

See `docs/EVIDENCE_MATRIX.md` for the repository-verifiable scope and current limitations.
