# Support

## Supported / verified scope

Verified by CI:
- host builds + unit tests on Windows, Linux, and macOS
- clang ASan/UBSan run on Ubuntu

Not verified by repository artifacts:
- embedded hardware behavior
- production RTOS backend behavior
- production MPU backend behavior
- companion-enabled builds (require external sources)

## Getting help

Please open an issue and include:
- OS + compiler + CMake version
- exact build commands and options (including any `LOXGUARD_USE_*` flags)
- whether you have companion sources under `ecosystem/` or `third_party/`
- `ctest --output-on-failure` output

## Evidence requests

If reporting embedded behavior or performance results, include raw logs and build output under `artifacts/evidence/` and reference them from `docs/EVIDENCE_MATRIX.md`.
