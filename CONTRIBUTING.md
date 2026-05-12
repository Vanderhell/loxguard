# Contributing

Thanks for considering contributing.

## Quick start

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## Coding expectations

- C99, portable where practical.
- Keep public API changes small and well-documented.
- Prefer small commits with clear, factual messages.
- Add tests for bug fixes and behavior changes where reasonable.

## Documentation and evidence discipline

- Do not add speculative claims to `README.md` or user-facing docs.
- If you document performance, platform behavior, or hardware results, include raw artifacts under `artifacts/evidence/` and reference them from `docs/EVIDENCE_MATRIX.md`.
- If a claim cannot be backed by code/tests or checked-in artifacts, mark it as not verified or move it to `docs/design/`.

## Reporting bugs

Please include:
- OS, compiler, and CMake version
- the exact configure/build/test commands used
- relevant logs (attach `ctest` output)
- a minimal repro if possible

## Pull requests

- Keep scope focused.
- Ensure CI passes.
- Update `docs/EVIDENCE_MATRIX.md` when the verification surface changes.
