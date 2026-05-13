# Component Integration Status

Date: 2026-05-13

This repository does not vendor companion libraries under `ecosystem/`. Optional integrations are enabled only when you provide the companion sources locally (under `ecosystem/` or the legacy fallback `third_party/`).

CI scope:
- CI verifies `default` and `no-ecosystem` configurations.
- Companion-enabled builds are not part of the default CI matrix because companion sources are not fetched.

## Optional integration switches

The following CMake options exist:

| Option | Purpose | Requires external sources | Verified in CI |
|---|---|---:|---:|
| `LOXGUARD_USE_NVLOG` | host persistence adapter path | yes (`nvlog`) | no |
| `LOXGUARD_USE_MICROTIMER` | timer adapter path | yes (`microtimer`) | no |
| `LOXGUARD_USE_MICROWDT` | watchdog/liveness adapter path | yes (`microwdt`) | no |
| `LOXGUARD_USE_MICRORES` | recovery/circuit-breaker adapter path | yes (`microres`) | no |
| `LOXGUARD_USE_MICROASSERT` | planned assert integration surface | yes (`microassert`) | no |
| `LOXGUARD_USE_PANICDUMP` | planned dump integration surface | yes (`panicdump`) | no |
| (others) | planned/experimental integration surfaces | yes | no |

Notes:
- If the ecosystem root is not present, loxguard builds core-only (no companion sources).
- If an option is enabled but the required companion files are not present under the discovered ecosystem root, the build continues without that integration and CMake emits a warning.
- Presence of an option does not imply embedded production readiness; see `docs/LIMITATIONS.md` and `docs/EVIDENCE_MATRIX.md`.

## Related docs

- `docs/INTEGRATION.md`
- `docs/EVIDENCE_MATRIX.md`
