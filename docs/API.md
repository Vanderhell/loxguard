# API

This file documents the public C API exposed by the headers in `include/`.

## Version

`include/loxguard.h` defines:
- `LOXGUARD_VERSION_MAJOR`, `LOXGUARD_VERSION_MINOR`, `LOXGUARD_VERSION_PATCH`

## Stability

For stability policy and what is considered stable starting at `v1.0.0`, see `docs/API_STABILITY.md`.

At a high level:
- **STABLE**: `loxguard.h`, `loxguard_checked.h`, `loxguard_format.h`
- **EXPERIMENTAL**: `loxguard_experimental.h` and all headers it includes
- **DEMO/TEST ONLY**: host demo helpers declared in `loxguard.h`

Experimental headers expose host stubs, adapter hooks, and optional integration
surfaces. They do not imply production support for RTOS, MPU, shell, or
companion-backed behavior.

## Core types

From `include/loxguard.h`:
- `loxguard_block_cfg_t`: Guard Block configuration. `input`, `output`, `scratch`, and `blackbox` are caller-owned and borrowed for the run.
- `lox_guard_ctx_t`: per-run context passed to a guarded function; the library owns the context object during the call.
- `lox_blackbox_t`: fixed-size in-memory event record; the blackbox owns copied event text.
- `lox_event_t`: structured event record; `block_name` and `reason` are borrowed pointers.
- `lox_report_t`: per-run summary (result, action, duration, persisted flag); string pointers are borrowed.

## Running a Guard Block

- `lox_report_t loxguard_run(const loxguard_block_cfg_t *cfg, loxguard_fn_t fn, void *user_ctx);`
- `typedef int (*loxguard_fn_t)(lox_guard_ctx_t *guard, void *user_ctx);`

Notes:
- The caller owns and initializes the blackbox (`lox_blackbox_init(...)`).
- Checked behavior only applies when code uses loxguard checked helpers (spans/arenas), not to arbitrary C pointer use.
- `lox_blackbox_store(...)` copies event text into blackbox-owned storage.
- `loxguard_run(...)` returns a report with borrowed string pointers; callers must not free or reuse them as owned memory.

## Checked spans

Span helpers provide bounds-checked reads/writes on `lox_span_t`:
- `lox_span_t lox_span_readonly(const void *ptr, size_t len);`
- `lox_span_t lox_span_writable(void *ptr, size_t len);`
- `lox_span_read_u8`, `lox_span_write_u8`, `lox_span_read`, `lox_span_write`, `lox_span_memcpy`

## Arena

Arena helpers provide bounded allocation from a fixed memory block:
- `lox_arena_init`, `lox_arena_alloc`, `lox_arena_reset`, `lox_arena_used`, `lox_arena_remaining`

## Blackbox and policy

- `lox_blackbox_init`, `lox_blackbox_store`
- `lox_action_t lox_policy_decide(const lox_event_t *event);`
- `void lox_set_recovery_callback(lox_recovery_cb_t cb, void *user_ctx);`

The recovery callback is notification-only:
- it receives the final event/action pair for a run,
- it does not own the event payload,
- it cannot propagate an error back into `loxguard`,
- it is not a cleanup/defer hook for caller resources.

Persistence adapters report failure explicitly through their return value. A
non-OK persistence status means the event was not durably stored, and
`lox_report_t.event_persisted` must not be treated as durable evidence when it
is `0`.

## Export/import formats (host diagnostics)

From `include/loxguard_format.h`:
- event lines: `lox_event_format_csv(...)`, `lox_event_parse_csv_line(_ex)(...)`
- blackbox CSV buffer/lines: `lox_blackbox_export_csv_*`, `lox_blackbox_import_csv_buffer(...)`
- report lines: `lox_report_format_kv(...)`, `lox_report_parse_kv(_ex)(...)`

Legacy `lox_report_parse_kv(...)` returns numeric/report fields only. Its text pointers are set to `NULL` after parse; use `lox_report_parse_kv_ex(...)` for an owning snapshot.

Schema and compatibility rules are defined in `docs/API_STABILITY.md` and `docs/FORMAT_EXPORTS.md`.

## Experimental headers

The following headers are intentionally **EXPERIMENTAL** for `v1.0.0`:

- `include/loxguard_adapters.h` (ecosystem integration hooks; optional/non-vendored)
- `include/loxguard_backends.h` (RTOS/MPU stub interfaces)
- `include/loxguard_ports.h` (port selection stubs)
- `include/loxguard_rtos_bridge.h` (RTOS bridging helpers; host-tested only)
- `include/loxguard_shell.h` (host shell command helper)
- `include/loxguard_profiles.h` (descriptive profile labels only; no API/source gating)

To opt in explicitly, include:

```c
#include "loxguard_experimental.h"
```

This defines `LOXGUARD_EXPERIMENTAL=1` for the translation unit.

## Demos (host-only, demo/test-only API)

The following helpers are shipped for demo/testing and are not evidence of embedded containment:
- `lox_run_checked_parser_demo(...)`
- `lox_run_checked_parser_timeout_demo(...)`
- `lox_run_guard_panic_demo(...)`
- `lox_run_guard_fault_demo(...)`
- `lox_run_rtos_timeout_demo(...)` (host stub/demo)
- `lox_run_mpu_fault_demo(...)` (host stub/demo)

The `LOX_PORT_FREERTOS_STUB` and `LOX_PORT_CORTEXM_STUB` values are synthetic
host-side mappers used in tests and demos. They are not claims of production
RTOS or MPU support.

## Related docs

- `docs/INTEGRATION.md`
- `docs/LIMITATIONS.md`
- `docs/EVIDENCE_MATRIX.md`
