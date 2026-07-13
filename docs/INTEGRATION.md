# Integration

This repository builds as a standalone CMake project and can also be consumed as a library.

Companion integrations (optional) are discovered only if you provide sources under `ecosystem/` (preferred) or `third_party/` (fallback).

## CMake: add_subdirectory

```cmake
add_subdirectory(path/to/loxguard)
target_link_libraries(my_target PRIVATE loxguard::loxguard)
```

Headers are under `include/`:

```c
#include "loxguard.h"
```

Notes:
- When loxguard is added as a subdirectory, `LOXGUARD_BUILD_TESTS` defaults to `OFF` to avoid building tests/demos in your superproject.

## CMake: install + find_package

If you install the project, a CMake package is generated:

```cmake
find_package(loxguard CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE loxguard::loxguard)
```

Installation exports the `loxguard::loxguard` target.

## Source-copy integration

If you do not want to use CMake, you can:
- add the files in `src/` to your build, and
- add `include/` to your compiler include paths.

Minimum required sources for the core library are the `.c` files listed in the top-level `CMakeLists.txt` target `loxguard`.

## Optional ecosystem integrations

The project does not vendor companion libraries. If the matching headers/sources are present in `ecosystem/` (or `third_party/`) then CMake can enable adapter paths using options:

- `LOXGUARD_USE_NVLOG` (host persistence adapter path; requires `nvlog`)
- `LOXGUARD_USE_MICROTIMER` (requires `microtimer`)
- `LOXGUARD_USE_MICROWDT` (requires `microwdt`)
- `LOXGUARD_USE_MICRORES` (requires `microres`)
- additional `LOXGUARD_USE_*` switches exist for planned integrations; see `CMakeLists.txt`

The host port selectors `LOX_PORT_FREERTOS_STUB` and `LOX_PORT_CORTEXM_STUB`
are synthetic mappers used in tests and demos. They are not production RTOS or
MPU support claims.

Verification scope:
- CI verifies `default` and `no-ecosystem` builds on Windows/Linux/macOS, plus a clang ASan/UBSan job on Ubuntu.
- Companion-enabled builds require companion sources and are not part of the default CI matrix.

Ecosystem root discovery order (when `LOX_ECOSYSTEM_DIR` is not set):
1. `${CMAKE_CURRENT_SOURCE_DIR}/ecosystem`
2. `${CMAKE_SOURCE_DIR}/ecosystem`
3. `${CMAKE_CURRENT_SOURCE_DIR}/third_party`
4. `${CMAKE_SOURCE_DIR}/third_party`

## Evidence discipline

When documenting platform behavior or performance results, add raw artifacts under `artifacts/evidence/` and reference them from `docs/EVIDENCE_MATRIX.md`.
