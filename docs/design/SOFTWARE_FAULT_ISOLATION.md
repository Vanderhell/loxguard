# Software Fault Isolation in loxguard

## 1. Definition

Software Fault Isolation means using software checks to restrict what a code region can access or do.

In loxguard, this becomes:

```text
Checked Guard Blocks
```

and later:

```text
Instrumented Guard Blocks
```

## 2. Why not start with compiler instrumentation

Automatic pointer instrumentation in C is hard.

Reasons:

- pointer provenance is difficult,
- aliasing is complex,
- raw pointer arithmetic is common,
- function calls can hide pointer origins,
- embedded toolchains vary,
- compiler plugins create adoption friction.

Therefore, MVP should not depend on LLVM/plugin instrumentation.

## 3. MVP SFI path

Use explicit contracts and checked APIs:

```text
span
arena
checked reads/writes
checked copies
bounds violation events
policy action
blackbox evidence
```

## 4. Future SFI path

Later `loxguard-instrument` may provide:

- Clang/LLVM pass,
- source rewriting,
- build-time checks,
- inserted `LOX_CHECK_PTR()` calls,
- call allowlists,
- forbidden function detection,
- automatic report generation.

## 5. Relationship to ASan/UBSan

loxguard is not a replacement for full sanitizers.

ASan/UBSan are excellent test/debug tools, but often too heavy for constrained production firmware.

loxguard targets smaller, explicit, production-friendly guarded regions.

## 6. Correct positioning

```text
loxguard does not try to sanitize the whole firmware.
It protects selected risky regions with explicit contracts and optional instrumentation.
```
