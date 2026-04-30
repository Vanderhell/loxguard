# loxguard

**Guard Blocks for embedded C.**

Wrap risky C code. Check its memory. Recover with evidence.

loxguard is a host-tested C99 MVP for Guard Blocks and Checked Guard Blocks.

It lets embedded C developers wrap risky code, such as parsers, protocol handlers, or optional modules, in supervised execution boundaries. Failures become structured events, policy decisions, and local blackbox evidence.

The current `v0.1.0-alpha` focuses on host-tested checked span/arena primitives, Guard Block lifecycle events, policy decisions, reports, and optional `nvlog` host persistence.

## Status

`v0.1.0-alpha` is a host-tested MVP.

Verified:
- Guard Block OK/failure lifecycle
- Checked span/arena primitives
- Policy + blackbox/report pipeline
- CSV/report import/export
- Optional nvlog host persistence
- Default, nvlog-enabled, and no-ecosystem builds

Not verified yet:
- Real embedded hardware behavior
- Production RTOS backend
- Production MPU backend
- LLVM/compiler instrumentation
- Production flash/EEPROM/FRAM persistence

## Quick Example

```c
#include "loxguard.h"

int main(void) {
    uint8_t in[8] = {0,1,2,3,4,5,6,7};
    uint8_t out[16] = {0};
    uint8_t scratch[64] = {0};
    lox_blackbox_t bb;
    lox_report_t report;

    report = lox_run_checked_parser_demo(
        in, sizeof(in),
        out, sizeof(out),
        scratch, sizeof(scratch),
        &bb
    );

    return (report.result == LOX_RESULT_OK) ? 0 : 1;
}
```

## Build and Test

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

no-ecosystem default:

```powershell
cmake -S . -B build_noeco
cmake --build build_noeco --config Debug
ctest --test-dir build_noeco -C Debug --output-on-failure
```

## CI Matrix

GitHub Actions workflow: `.github/workflows/ci.yml`

- `default`: configure/build/test default path
- `nvlog`: configure/build/test with `LOXGUARD_USE_NVLOG=ON`
- `noecosystem`: verifies default build/test with `ecosystem/` absent

## Automated Releases

Workflow: `.github/workflows/release.yml`

- runs automatically on pushed tags matching `v*`
- verifies before publish:
  - default configure/build/test
  - nvlog-enabled configure/build/test
  - no-ecosystem configure/build/test
- creates release assets:
  - `loxguard-<tag>.zip`
  - `loxguard-<tag>.zip.sha256`
- creates GitHub Release automatically and uploads both assets

## Companion Libraries

- Canonical companion path: `ecosystem/`
- Companion modules are optional ecosystem integrations.
- Current active integration: `safemath`, `microlog`, partial `microhealth`, optional host `nvlog`.

## Links

- `CHANGELOG.md`
- `docs/RELEASE_NOTES_v0.1.0-alpha.md`
- `docs/EVIDENCE_MATRIX.md`
- `docs/COMPONENT_INTEGRATION_STATUS.md`
