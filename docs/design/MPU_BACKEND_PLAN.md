# MPU Backend Plan

## Purpose

MPU backend upgrades Guard Blocks into hardware-contained failure boundaries where possible.

## Initial target

```text
Cortex-M4 with MPU
```

## Flow

```text
Guard Block memory violation
→ MemManage fault
→ fault detail captured
→ BLOCK_MEMORY_FAULT event
→ policy decision
→ quarantine/disable/recover
→ blackbox evidence
```

## Requirements

- real hardware validation,
- QEMU is not final proof,
- no direct claim for all Cortex-M,
- DMA limitation documented,
- trusted drivers remain trusted.

## Non-goals

- all-MCU support,
- ESP32 support,
- RISC-V PMP support,
- dynamic loading,
- signed modules,
- TrustZone initial support.

## Positioning

```text
MPU backend is optional.
loxguard-core must be useful without it.
```
