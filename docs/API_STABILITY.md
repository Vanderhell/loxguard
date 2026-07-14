# API Stability (v1.0.0 policy)

This document defines what is **stable** starting with `v1.0.0`, what is **experimental**, and what is **demo/test-only**.

The intended `v1.0.0` claim is:

> Stable host-tested C99 Guard Block + Checked Span/Arena + Blackbox Evidence core.

Anything outside that scope must remain **EXPERIMENTAL** or **NOT VERIFIED** unless this repository contains raw evidence artifacts proving the claim (see `docs/EVIDENCE_MATRIX.md`).

## Semantic versioning policy

loxguard follows SemVer starting at `v1.0.0`.

- **MAJOR** (`2.0.0`): breaking changes to the **STABLE** API surface are allowed.
- **MINOR** (`1.x.0`): new functionality may be added in a backwards compatible way; **STABLE** APIs must remain source-compatible.
- **PATCH** (`1.0.x`): bugfixes only; **STABLE** API and documented formats must remain compatible.

Experimental and demo/test-only APIs may change in any `1.x` release.

## Header stability classes

STABLE headers (public contract from `v1.0.0`):
- `include/loxguard.h` (core types + Guard Block runner + span/arena/blackbox)
- `include/loxguard_checked.h` (checked helper macros for spans/arena)
- `include/loxguard_format.h` (host diagnostic export/import formats; see below)

Stable blackbox layout macros:
- `LOX_BLACKBOX_MAX_EVENTS` = `16`
- `LOX_BLACKBOX_STR_MAX` = `64`

Consumer predefinitions that attempt to change either value are rejected at compile time.

EXPERIMENTAL headers (not part of the v1 stable contract):
- `include/loxguard_experimental.h` (umbrella header that defines `LOXGUARD_EXPERIMENTAL=1`)
- `include/loxguard_adapters.h`
- `include/loxguard_backends.h`
- `include/loxguard_ports.h`
- `include/loxguard_rtos_bridge.h`
- `include/loxguard_shell.h`
- `include/loxguard_profiles.h` (descriptive profile labels only; no API/source gating)

Capability classes used in this repository:

- host stub / synthetic mapper: test-only routing such as `LOX_PORT_FREERTOS_STUB`
  and `LOX_PORT_CORTEXM_STUB`
- compile-tested adapter: optional companion-backed adapter code that compiles in
  a host build when the companion sources are present
- real platform backend: a backend that runs on target hardware, not just on the host
- hardware-verified containment: only claim this when raw artifacts are checked in
  and referenced from `docs/EVIDENCE_MATRIX.md`

Demo/test-only APIs (not a stable contract):
- Host demo helpers declared in `include/loxguard.h` (see `docs/API.md`)

## Stable API surface (v1.0.0)

The following categories are considered **STABLE** starting at `v1.0.0`:

- Guard Block execution: `loxguard_run(...)`, `loxguard_block_cfg_t`, `lox_guard_ctx_t`
- Evidence core: `lox_blackbox_t`, `lox_blackbox_init(...)`, `lox_blackbox_store(...)`
- Event/report types: `lox_event_t`, `lox_report_t`, `lox_policy_decide(...)`
- Checked span helpers: `lox_span_t` and the `lox_span_*` functions
- Checked arena helpers: `lox_arena_t` and the `lox_arena_*` functions

Notes:
- Checked behavior applies only when user code uses these span/arena APIs (or macros wrapping them). Arbitrary pointer use in C is not automatically checked.
- RTOS/MPU and ecosystem integrations are not part of the v1 stable contract.

Ownership and callback notes:

- `lox_event_t.block_name` and `lox_event_t.reason` are borrowed pointers.
- `lox_report_t.last_block`, `lox_report_t.last_failed_block`, and `lox_report_t.reason` are borrowed pointers.
- `lox_blackbox_store(...)` copies event text into blackbox-owned storage.
- `loxguard_run(...)` borrows caller-owned config/scratch/blackbox inputs for the duration of the call.
- `lox_set_recovery_callback(...)` installs a notification-only hook; it is not a cleanup/defer mechanism and cannot propagate an error back into `loxguard`.
- A persistence failure must be treated as non-durable evidence; `event_persisted=0` means the event was not durably stored.

## Export/import format stability (v1.0.0)

loxguard exposes two line-oriented, fixed-buffer formats intended for host diagnostics:

- Event line: `lox_event_format_csv(...)` and `lox_event_parse_csv_line(_ex)(...)`
- Report line: `lox_report_format_kv(...)` and `lox_report_parse_kv(_ex)(...)`

Starting at `v1.0.0`, the **format schema and parsing rules are stable** for `1.x`:

### Event line schema (v1)

- Optional header line (when exported via `lox_blackbox_export_csv_buffer_ex(..., include_header=1, ...)`):
  - `kind,block,reason,index,limit,aux`
- Data line (strict key order, comma-separated, percent-encoded values):
  - `kind=<int>,block=<text>,reason=<text>,index=<size_t>,limit=<size_t>,aux=<uint>`

### Report line schema (v1)

- Data line (strict key order, comma-separated, percent-encoded values):
  - `block=<text>,reason=<text>,result=<uint>,action=<uint>,event_kind=<uint>,duration_ticks=<uint>,event_persisted=<0|1>`

### Escaping / percent-encoding (stable)

To keep lines parseable and reversible, `block` / `reason` values are percent-encoded for a reserved set:

- `,`  -> `%2C`
- `\n` -> `%0A`
- `\r` -> `%0D`
- `=`  -> `%3D`
- `%`  -> `%25`

### Truncation and parsing rules (stable)

- All formatters are fixed-buffer and always NUL-terminate `out` when `out_len > 0`.
- If the output would exceed `out_len`, the output is truncated and the returned length is `out_len - 1`.
- Parsers are **strict**:
  - unknown keys/fields are not accepted,
  - key order must match the schema,
  - trailing garbage/whitespace is rejected.
- Snapshot structs store decoded `block_name` and `reason` in fixed-size `64` byte buffers (including the terminator). Long decoded values are truncated.

### Compatibility policy

- `v1.x` will not change the v1 schema or parsing rules.
- Any incompatible change (including adding new fields to the schema) requires a format version bump and corresponding new APIs in a future major release.

## Deprecation policy

For the **STABLE** API surface:
- Deprecations will be documented and, when feasible, kept for at least one MINOR release before removal in the next MAJOR version.

For experimental/demo APIs:
- No deprecation guarantees are made; they may change without notice in `1.x`.

Profile labels in `include/loxguard_profiles.h` are descriptive only. They do
not gate stable headers or source files, and they do not upgrade stub/demo
paths into production claims.
