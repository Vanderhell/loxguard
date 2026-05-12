# loxguard

C99 library for running user code in a supervised “guard block” that produces structured events and a small local evidence record.

## Status

- Current stage: `v0.1.0-alpha` (host-tested; no certification claims)
- Verified by repository evidence: host builds + unit tests (see `docs/EVIDENCE_MATRIX.md`)
- Embedded/RTOS/MPU behavior: interface stubs and demos exist, but hardware behavior is not verified by artifacts in this repository

## What it does

- Runs a function inside a Guard Block wrapper (`loxguard_run(...)`)
- Provides checked span/arena helpers for bounds-checked access used by the demos/tests
- Captures lifecycle and failure events into a small in-memory “blackbox” (`lox_blackbox_t`)
- Formats and parses CSV/report exports (`docs/FORMAT_EXPORTS.md`)

## What it does not do

- It does not make arbitrary C code memory-safe.
- It does not provide RTOS/MPU “containment” in this alpha; those paths are not verified by repository artifacts.
- It does not claim safety/security certification or compliance.

## Quick example

Guard Block wrapper:

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
    lox_blackbox_init(&bb);
    memset(&cfg, 0, sizeof(cfg));
    cfg.name = "my_block";
    cfg.timeout_ms = 20;
    cfg.criticality = LOXGUARD_OPTIONAL;
    cfg.max_failures = 3;
    cfg.blackbox = &bb;
    return (loxguard_run(&cfg, my_block, NULL).result == LOX_RESULT_OK) ? 0 : 1;
}
```

Checked demo (host):

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

## Build and test

Default:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

No-ecosystem verification (CI deletes `ecosystem/`):

```powershell
cmake -S . -B build_noeco
cmake --build build_noeco --config Debug
ctest --test-dir build_noeco -C Debug --output-on-failure
```

Sanitizers (CI, Ubuntu/clang only):

```bash
cmake -S . -B build_san \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_FLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build_san -j
ctest --test-dir build_san --output-on-failure
```

## Integration

See `docs/INTEGRATION.md` for:
- using `add_subdirectory(...)`
- install + `find_package(...)`
- source-copy integration
- optional ecosystem integrations (`LOXGUARD_USE_*`)

## Tested platforms (repository evidence)

- CI runs build+test on: Windows, Linux, macOS
- CI runs a clang ASan/UBSan job on Ubuntu

## Evidence

- Evidence matrix: `docs/EVIDENCE_MATRIX.md`
- Raw host logs (when checked in): `artifacts/evidence/host/`
- Hardware evidence directory (currently placeholders): `artifacts/evidence/esp32/`

## Documentation

- Public API: `docs/API.md`
- Architecture: `docs/ARCHITECTURE.md`
- Integration: `docs/INTEGRATION.md`
- Limitations: `docs/LIMITATIONS.md`
- Release notes: `docs/RELEASE_NOTES_v0.1.0-alpha.md`
- Design notes (non-current): `docs/design/`

## License

See `LICENSE`.
