# Prompt 16 — Instrumentation roadmap

## Task

Plan future instrumentation without implementing it.

## Required output

Create/update:

```text
docs/INSTRUMENTATION_ROADMAP.md
docs/SOFTWARE_FAULT_ISOLATION.md
```

## Required phases

- manual checked API,
- macro-assisted checked C,
- static source checks,
- Clang/LLVM instrumentation,
- hardware/VM backends.

## Must state

Compiler instrumentation is not MVP.

Manual span/arena API is MVP.

## Do not

- Do not implement plugin.
- Do not require LLVM.
- Do not overclaim.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
