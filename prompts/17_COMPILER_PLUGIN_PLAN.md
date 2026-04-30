# Prompt 17 — Compiler plugin plan

## Task

Plan optional future compiler/tooling support.

## Required output

Create/update:

```text
docs/COMPILER_PLUGIN_PLAN.md
```

## Required sections

- goals,
- possible tool options,
- desired checks,
- hard problems,
- realistic first checker,
- later LLVM plugin.

## First realistic tool

Plan `loxguard-check`, not full instrumentation.

It may check:

- no malloc/free in guarded files,
- no raw memcpy/memset unless allowed,
- guarded files use span/arena API,
- forbidden calls,
- report generation.

## Do not

- Do not implement the plugin.
- Do not make plugin required for MVP.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
