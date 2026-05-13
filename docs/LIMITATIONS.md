# Limitations

## Soft Guard Blocks

Soft Guard Blocks do not make C memory-safe.

They cannot guarantee containment of arbitrary pointer corruption.

They are useful for:

- explicit errors,
- controlled failures,
- timeout/liveness failures,
- panic/assert integration,
- evidence and recovery policy.

## Hardware containment

Memory fault containment requires hardware support:

- MPU,
- MMU,
- PMP,
- VM/interpreter,
- or equivalent isolation.

Repository evidence note:
- This repository currently verifies the **host-tested core** behavior only (see `docs/EVIDENCE_MATRIX.md`).
- RTOS/MPU “containment” interfaces and demos do not constitute verified embedded behavior unless raw hardware artifacts are checked into `artifacts/evidence/`.

## Not covered by default

- DMA misuse,
- trusted driver bugs,
- physical attacks,
- all undefined behavior,
- corrupted privileged runtime,
- certification.

## Correct claim

```text
Detect and recover everywhere. Contain where hardware allows.
```

## Checked Guard Block limitations

Checked Guard Blocks only protect accesses that go through loxguard's checked APIs or future instrumentation.

They cannot stop arbitrary raw pointer writes in normal C code.

Correct:

```text
Checked Guard Blocks detect bounds violations in checked spans/arenas.
```

Incorrect:

```text
Checked Guard Blocks automatically sandbox arbitrary C code without instrumentation.
```
