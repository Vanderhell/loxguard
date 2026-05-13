# loxguard

Host-tested C99 library for running user code in a supervised "guard block" that produces structured events and a small local evidence record.

## Status (v1.0.0 release candidate scope)

`v1.0.0` is intended to be a stable public contract for:

- Guard Block execution (`loxguard_run(...)`)
- Checked Span/Arena helpers (`lox_span_*`, `lox_arena_*`, and `loxguard_checked.h` macros)
- Blackbox evidence core (`lox_blackbox_t`, events, reports)
- Host diagnostic export/import formats (`loxguard_format.h`) as documented in `docs/API_STABILITY.md`

Not verified by repository evidence (unless raw artifacts exist under `artifacts/evidence/`):

- embedded hardware behavior (ESP32 or other MCU)
- power-loss / restart consistency
- production RTOS backend behavior
- production MPU backend behavior
- companion-enabled ecosystem builds (unless companion sources are checked in and exercised by CI)

Verified-by-repo scope is tracked in `docs/EVIDENCE_MATRIX.md`.

## What it does

- Runs a function inside a Guard Block wrapper (`loxguard_run(...)`)
- Provides checked span/arena helpers for bounds-checked access (opt-in through the checked APIs)
- Captures lifecycle and failure events into a small in-memory blackbox (`lox_blackbox_t`)
- Formats and parses host diagnostic exports (`docs/FORMAT_EXPORTS.md`)

## What it does not do

- It does not make arbitrary C code memory-safe.
- It does not claim embedded RTOS/MPU "containment" as verified behavior without raw hardware artifacts in this repository.
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

Checked span use (opt-in checked API):

```c
#include "loxguard.h"
#include "loxguard_checked.h"

int main(void) {
    uint8_t out[2] = {0};
    lox_span_t s = lox_span_writable(out, sizeof(out));
    return (LOX_WRITE_U8(&s, 0, 0xAA) == LOXGUARD_OK) ? 0 : 1;
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

## Integration

See `docs/INTEGRATION.md` for:

- using `add_subdirectory(...)`
- install + `find_package(loxguard CONFIG REQUIRED)`
- optional ecosystem integrations (`LOXGUARD_USE_*`)

## Evidence and limitations

- Evidence matrix: `docs/EVIDENCE_MATRIX.md`
- Limitations: `docs/LIMITATIONS.md`

## Documentation

- API: `docs/API.md`
- API stability: `docs/API_STABILITY.md`
- Integration: `docs/INTEGRATION.md`
- Release notes (v1): `docs/RELEASE_NOTES_v1.0.0.md`

## License

See `LICENSE`.
