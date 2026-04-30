# Instrumentation Roadmap

## Phase A — Manual Checked API

MVP.

- spans,
- arenas,
- checked reads/writes,
- checked copy/set,
- bounds violation events,
- blackbox report.

## Phase B — Macro-assisted Checked C

Improve ergonomics.

Possible helpers:

```c
LOX_READ_U8(span, i, out)
LOX_WRITE_U8(span, i, value)
LOX_MEMCPY(dst, dst_off, src, src_off, len)
LOX_ARENA_ALLOC(arena, type, count)
```

## Phase C — Static checks

Build-time checks without full compiler plugin:

- forbid malloc/free in guarded files,
- forbid raw memcpy/memset in guarded files,
- require loxguard span API,
- grep/clang-tidy style checks,
- call allowlist.

## Phase D — Clang/LLVM instrumentation

Future.

Potential capabilities:

- insert load/store checks,
- detect forbidden global writes,
- enforce call allowlist,
- generate memory access report,
- fail build on unsafe patterns.

## Phase E — Hardware/VM backends

- MPU,
- PMP,
- WASM/bytecode,
- interpreter sandbox.

## Rule

Do not block MVP on Phase D.
