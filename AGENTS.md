# AGENTS.md — loxguard Guard Blocks v4

## Mission

Design and build `loxguard` as **Guard Blocks for embedded C**, extended with **Checked Guard Blocks**.

The product goal is:

```text
Let embedded C developers wrap risky code in supervised failure boundaries, optionally with checked memory contracts.
```

When a Guard Block fails, loxguard must turn that failure into:

```text
structured event
→ policy decision
→ recovery action
→ blackbox evidence
→ inspectable report
```

## Product slogan

```text
Wrap risky C code. Check its memory. Recover with evidence.
```

## Core product model

```text
Guard Block
    ↓
optional memory contract / checked span / arena
    ↓
detect
    ↓
classify
    ↓
decide
    ↓
act
    ↓
persist
    ↓
inspect
```

## Guard Block levels

### Level 1 — Soft Guard Blocks

Portable.

Can manage:

- return-code failures,
- controlled `loxguard_fail()` failures,
- panic/assert integration,
- timeout/liveness failures,
- policy decisions,
- blackbox evidence.

Cannot guarantee arbitrary memory-corruption containment.

### Level 2 — Checked Guard Blocks

Portable, explicit-contract based.

Can manage:

- checked input spans,
- checked output spans,
- scratch arenas,
- checked reads/writes,
- checked memcpy/memset helpers,
- bounds violation events,
- arena overflow events,
- stack/recursion budget tracking where feasible.

This is the first important MVP after basic Guard Blocks.

### Level 3 — Instrumented Guard Blocks

Future toolchain-assisted mode.

May use:

- Clang/LLVM instrumentation,
- source rewrite,
- macro-assisted wrappers,
- static analysis,
- call allowlists,
- automatic guard reports.

This is **not MVP**.

### Level 4 — RTOS Guard Blocks

Optional.

Maps guarded work to tasks/jobs where an RTOS exists.

### Level 5 — Hardware-contained Guard Blocks

Optional.

Uses MPU/MMU/PMP/VM where available.

Initial future target:

```text
Cortex-M with MPU
```

## What loxguard owns

loxguard owns:

- Guard Block API,
- Checked Guard Block API,
- memory contract model,
- span/arena checked API,
- module/block model,
- failure pipeline,
- unified event schema,
- policy engine,
- local blackbox,
- recovery modes,
- profile composition,
- adapter layer,
- evidence reports,
- optional instrumentation roadmap,
- optional RTOS backend,
- optional MPU backend.

## Existing libraries

Existing libraries are reusable accelerators, not mandatory boundaries.

Consider reusing:

- microhealth,
- microwdt,
- microboot,
- microassert,
- microlog,
- nvlog,
- microtimer,
- microbus,
- panicdump,
- microflash,
- microres,
- microconf,
- microsh,
- microcbor,
- microtest,
- safemath,
- loxdb optionally.

## Reuse rule

Prefer existing libraries when they fit the Guard Block/failure-pipeline model.

Do not duplicate them unnecessarily.

If an existing library does not fit cleanly, create a small adapter or a loxguard-owned component.

Do not distort the Guard Block model just to reuse a library.

## MVP direction

The first strong MVP must be host-testable.

Do not require MPU, RTOS, cloud, OTA, signed modules, or LLVM plugin.

MVP scenario:

```text
packet_parser Guard Block enters
→ parser uses checked input/output spans
→ parser tries out-of-bounds output write
→ loxguard detects bounds violation before write
→ event created
→ policy selects DROP_INPUT
→ blackbox stores evidence
→ report shows last_failed_block=packet_parser reason=BOUNDS
```

## Critical claims

Correct:

```text
Detect and recover everywhere. Check where contracts exist. Contain where hardware allows.
```

Incorrect:

```text
loxguard catches all C bugs everywhere.
loxguard makes C memory-safe.
loxguard is Docker for MCU.
compiler plugin is MVP.
```

## First phase

Before implementation:

1. define Guard Block API,
2. define Checked Guard Block memory contracts,
3. define span/arena API,
4. define failure pipeline,
5. define event schema,
6. define policy engine,
7. define blackbox,
8. inventory reusable libraries,
9. map reuse,
10. define checked parser MVP,
11. define tests/evidence.

## Forbidden unless explicitly requested

Do not implement:

- full RTOS,
- cloud service,
- SaaS dashboard,
- dynamic module loading,
- signed modules,
- OTA rollback implementation,
- MPU backend implementation,
- LLVM/compiler plugin implementation,
- new database,
- new logger if an existing one fits,
- new watchdog if an existing one fits,
- marketing website,
- unrelated refactoring.

## Engineering constraints

- C99.
- no heap by default.
- no external dependencies by default.
- static allocation preferred.
- deterministic behavior preferred.
- host-testable core.
- embedded-portable core.
- optional hardware-specific backends.
- claims must be backed by evidence.

## Git rules

Do not create git commits unless explicitly asked.

Do not add Codex, OpenAI, ChatGPT, Claude, or any AI tool as Author or Co-Author in any commit message.

## Final response format

Every final response must use exactly:

```text
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
```
