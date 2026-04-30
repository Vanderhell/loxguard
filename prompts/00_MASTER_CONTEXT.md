# Prompt 00 — Master context

You are working on `loxguard`.

Current product identity:

```text
loxguard = Guard Blocks for embedded C
          + Checked Guard Blocks
          + failure-management pipeline
          + policy engine
          + local blackbox evidence
```

Developer-facing slogan:

```text
Wrap risky C code. Check its memory. Recover with evidence.
```

## Core concept

Developers wrap risky code with `loxguard_run()`.

Checked Guard Blocks add explicit memory contracts:

```text
input spans
output spans
scratch arena
stack/recursion budgets later
allowed calls later
```

When a block fails or violates a memory contract, loxguard creates an event, applies policy, stores blackbox evidence, and returns a controlled result.

## First strong MVP

Host-testable.

No MCU required.

No MPU required.

No RTOS required.

No LLVM/compiler plugin required.

Expected scenario:

```text
packet_parser Guard Block enters
→ parser uses checked output span
→ parser tries write at index 17 into len 16
→ loxguard prevents write
→ event BLOCK_WRITE_OUT_OF_BOUNDS created
→ policy selects DROP_INPUT
→ blackbox stores event
→ report shows last_failed_block=packet_parser reason=BOUNDS
```

## Existing libraries

Existing libraries are accelerators, not boundaries.

Reuse when useful.

Do not distort the Guard Block product model to fit an existing library.

## Rules

- C99.
- no heap by default.
- no external dependencies by default.
- deterministic behavior.
- host-testable core.
- optional compiler instrumentation later.
- optional RTOS/MPU backends later.
- do not claim memory safety everywhere.
- do not implement LLVM/plugin first.
- do not implement MPU first.
- do not create commits unless explicitly requested.

## First task

Validate/update the Guard Blocks + Checked Guard Blocks product specification.

Do not implement code yet.

## Final output format

Use exactly:

```text
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
```
