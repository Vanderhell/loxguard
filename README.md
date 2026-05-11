# loxguard

**Guard Blocks for embedded C.**

Wrap risky C code. Check its memory. Recover with evidence.

loxguard is a host-tested C99 MVP for Guard Blocks and Checked Guard Blocks.

It lets embedded C developers wrap risky code, such as parsers, protocol handlers, or optional modules, in supervised execution boundaries. Failures become structured events, policy decisions, and local blackbox evidence.

The current `v0.1.0-alpha` focuses on host-tested checked span/arena primitives, Guard Block lifecycle events, policy decisions, reports, and optional `nvlog` host persistence.

Non-goals and claims (important):
- This project does **not** claim memory safety for arbitrary C code.
- This project does **not** provide safety certification, compliance guarantees, or “secure by default” claims.
- Checked behavior exists only where explicit contracts/spans/arenas are used.

## Status

`v0.1.0-alpha` is a host-tested MVP.

Verified:
- Guard Block OK/failure lifecycle
- Checked span/arena primitives
- Policy + blackbox/report pipeline
- CSV/report import/export
- Optional nvlog host persistence
- Optional microtimer/microwdt integration paths
- Optional microres recovery/circuit-breaker adapter path
- Host panic/fault evidence paths (`BLOCK_PANIC`, `BLOCK_FAULT`)
- Default, nvlog-enabled, and no-ecosystem builds

Not verified yet:
- Real embedded hardware behavior
- Production RTOS backend
- Production MPU backend
- LLVM/compiler instrumentation
- Production flash/EEPROM/FRAM persistence
- Companion `microassert` runtime integration (module not present in current workspace)
- Companion `panicdump` runtime integration (module not present in current workspace)

## Quick Example

Generic Guard Block wrapper:

```c
#include "loxguard.h"
#include <string.h>

static int my_block(lox_guard_ctx_t *g, void *user_ctx) {
    (void)g;
    (void)user_ctx;
    return LOXGUARD_OK;
}

int main(void) {
    lox_blackbox_t bb;
    loxguard_block_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.name = "my_block";
    cfg.timeout_ms = 20;
    cfg.criticality = LOXGUARD_OPTIONAL;
    cfg.max_failures = 3;
    cfg.blackbox = &bb;
    return (loxguard_run(&cfg, my_block, NULL).result == LOX_RESULT_OK) ? 0 : 1;
}
```

Checked parser demo:

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

microtimer-enabled:

```powershell
cmake -S . -B build_mtimer -DLOXGUARD_USE_MICROTIMER=ON
cmake --build build_mtimer --config Debug
ctest --test-dir build_mtimer -C Debug --output-on-failure
```

microwdt-enabled:

```powershell
cmake -S . -B build_mwdt -DLOXGUARD_USE_MICROWDT=ON
cmake --build build_mwdt --config Debug
ctest --test-dir build_mwdt -C Debug --output-on-failure
```

microtimer+microwdt:

```powershell
cmake -S . -B build_mtimer_mwdt -DLOXGUARD_USE_MICROTIMER=ON -DLOXGUARD_USE_MICROWDT=ON
cmake --build build_mtimer_mwdt --config Debug
ctest --test-dir build_mtimer_mwdt -C Debug --output-on-failure
```

microres-enabled:

```powershell
cmake -S . -B build_mres -DLOXGUARD_USE_MICRORES=ON
cmake --build build_mres --config Debug
ctest --test-dir build_mres -C Debug --output-on-failure
```

no-ecosystem default:

```powershell
cmake -S . -B build_noeco
cmake --build build_noeco --config Debug
ctest --test-dir build_noeco -C Debug --output-on-failure
```

## CI Matrix

GitHub Actions workflow: `.github/workflows/ci.yml`

- `default`: configure/build/test (Windows, Linux, macOS)
- `noecosystem`: verifies default build/test with `ecosystem/` absent (Windows, Linux, macOS)
- `release`: on `v*` tags, re-verifies default + no-ecosystem before publishing release assets

## Automated Releases

Workflow: `.github/workflows/release.yml`

- runs automatically on pushed tags matching `v*`
- verifies before publish:
  - default configure/build/test
  - no-ecosystem configure/build/test
- creates release assets:
  - `loxguard-<tag>.zip`
  - `loxguard-<tag>.zip.sha256`
- creates GitHub Release automatically and uploads both assets

## Companion Libraries

- Canonical companion path: `ecosystem/`
- Companion modules are optional ecosystem integrations.
- Current active integration: `safemath`, `microlog`, partial `microhealth`, optional host `nvlog`, optional `microtimer`, optional `microwdt`, optional `microres`.
- Watchdog late/starved semantics in this wave are represented through adapter watchdog state, not standalone `lox_event_t` kinds.

## Links

- `CHANGELOG.md`
- `docs/RELEASE_NOTES_v0.1.0-alpha.md`
- `docs/EVIDENCE_MATRIX.md`
- `docs/FORMAT_EXPORTS.md`
- `docs/API_STABILITY.md`
- `docs/COMPONENT_INTEGRATION_STATUS.md`
