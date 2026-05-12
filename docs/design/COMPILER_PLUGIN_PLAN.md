# Compiler Plugin Plan

## Status

Future only.

Not MVP.

## Goal

Automatically instrument selected Guard Blocks at compile time.

## Possible implementation options

1. Clang plugin.
2. LLVM pass.
3. Source-to-source rewriter.
4. clang-tidy style checker.
5. Build wrapper that enforces guarded-code rules.

## Desired capabilities

- insert bounds checks around loads/stores,
- detect raw pointer writes,
- enforce allowed memory regions,
- enforce call allowlists,
- detect forbidden functions,
- report stack allocations,
- detect recursion if possible,
- generate guard report.

## Hard problems

- pointer aliasing,
- pointer provenance,
- function pointers,
- inline assembly,
- volatile/MMIO,
- callbacks,
- companion ecosystem headers,
- embedded compiler differences.

## Initial realistic tool

Do not start with full instrumentation.

Start with a checker:

```text
loxguard-check
```

Checks:

- guarded files do not call malloc/free,
- guarded files do not call raw memcpy/memset unless allowed,
- guarded files use span/arena API,
- guarded files do not access known forbidden globals,
- guarded files produce report.

## Later tool

```text
loxguard-clang
```

Only after API and MVP prove useful.
