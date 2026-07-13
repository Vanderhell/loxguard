# Component Integration Status

Date: 2026-05-13

This repository does not vendor companion libraries under `ecosystem/`.
Optional integrations are enabled only when you provide sources locally under
`ecosystem/` (preferred) or `third_party/` (fallback).

## Capability classes

- Host stub / synthetic mapper: host-side routing used in tests and demos only.
- Compile-tested adapter: optional companion-backed code that builds when the
  companion sources are present.
- Real platform backend: a backend that runs on target hardware, not just the host.
- Hardware-verified containment: only claim this when raw artifacts are checked
  into `artifacts/evidence/` and referenced from `docs/EVIDENCE_MATRIX.md`.

## CMake option status

| Option | Capability class | Purpose | Requires external sources | Verified in CI |
|---|---|---|---|---|
| `LOXGUARD_USE_NVLOG` | Compile-tested adapter | host persistence adapter path | yes (`nvlog`) | no |
| `LOXGUARD_USE_MICROTIMER` | Compile-tested adapter | timer adapter path | yes (`microtimer`) | no |
| `LOXGUARD_USE_MICROWDT` | Compile-tested adapter | watchdog/liveness adapter path | yes (`microwdt`) | no |
| `LOXGUARD_USE_MICRORES` | Compile-tested adapter | recovery/circuit-breaker adapter path | yes (`microres`) | no |
| `LOXGUARD_USE_MICROASSERT` | Experimental/incomplete | planned assert integration surface | yes (`microassert`) | no |
| `LOXGUARD_USE_PANICDUMP` | Experimental/incomplete | planned dump integration surface | yes (`panicdump`) | no |
| `LOXGUARD_USE_MICROBOOT` | Experimental/incomplete | planned boot integration surface | yes | no |
| `LOXGUARD_USE_MICROBUS` | Experimental/incomplete | planned event-bus integration surface | yes | no |
| `LOXGUARD_USE_MICROCBOR` | Experimental/incomplete | planned serialization integration surface | yes | no |
| `LOXGUARD_USE_MICROFLASH` | Experimental/incomplete | planned flash integration surface | yes | no |
| `LOXGUARD_USE_MICROSH` | Experimental/incomplete | planned shell integration surface | yes | no |
| `LOXGUARD_USE_MICRORING` | Experimental/incomplete | planned ring-buffer integration surface | yes | no |
| `LOXGUARD_USE_MICROCONF` | Experimental/incomplete | planned config integration surface | yes | no |
| `LOXGUARD_USE_MICROTEST` | Experimental/incomplete | planned smoke-test integration surface | yes | no |
| `LOXGUARD_USE_LOXDB` | Compile-tested adapter | event-history adapter path | yes (`loxdb`) | no |

## Notes

- If the ecosystem root is not present, loxguard builds core-only.
- If an option is enabled but the required companion files are not present
  under the discovered ecosystem root, the build continues without that
  integration and CMake emits a warning.
- `LOX_PORT_FREERTOS_STUB` and `LOX_PORT_CORTEXM_STUB` are synthetic host-side
  mappers. They are useful for tests and demos, not production RTOS/MPU claims.
- Presence of an option does not imply embedded production readiness; see
  `docs/LIMITATIONS.md` and `docs/EVIDENCE_MATRIX.md`.

## Related docs

- `docs/INTEGRATION.md`
- `docs/EVIDENCE_MATRIX.md`
