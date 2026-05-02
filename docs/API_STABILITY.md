# API Stability

loxguard is currently **v0.x** and the API is not yet stable.

## What to expect in v0.x

- Breaking changes may occur between minor versions.
- Types, enums, and function signatures may change as the Guard Block + Checked Guard Block model stabilizes.
- Export/import formats are intended for host MVP diagnostics and may evolve.

## Stability goals (toward v1.0)

Before declaring `v1.0`, loxguard aims to:

- Freeze the public headers under `include/` (documented change policy).
- Define a compatibility story for reports/events (including any export/import formats).
- Provide a clear deprecation path for any replaced symbols.

## PRO / evidence tooling direction (non-binding)

The MIT core keeps fixed-size snapshots for deterministic, heap-free behavior.

Future “PRO/evidence/audit” tooling may introduce:

- larger or caller-provided decode buffers for `block_name`/`reason`,
- non-truncating exports (streaming),
- artifact bundling and stronger provenance workflows.
