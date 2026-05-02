# Evidence Artifacts

This folder contains raw, reviewable evidence outputs that back documentation claims (especially `docs/EVIDENCE_MATRIX.md`).

## Layout

- `artifacts/evidence/host/`
  - host build/test logs (CMake + ctest output)
- `artifacts/evidence/esp32/`
  - optional external logs from real hardware runs (bench output, SD log tails, footprint reports)

## Note

Host evidence should be checked in when making verification claims.

Hardware evidence is optional and may be omitted from the repo; if present, keep it in `artifacts/evidence/esp32/` and reference the filenames from `docs/EVIDENCE_MATRIX.md`.
