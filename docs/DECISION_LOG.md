# Decision Log v3

## Decision 001 — Guard Blocks first

Status: accepted

loxguard is primarily developer-facing Guard Blocks for embedded C.

The failure-management pipeline exists underneath Guard Blocks.

## Decision 002 — Existing libraries are accelerators

Status: accepted

Existing libraries should be reused where they fit, but they do not define product boundaries.

## Decision 003 — MPU is optional

Status: accepted

MPU backend is high-end containment, not core product.

## Decision 004 — Host MVP first

Status: accepted

First proof must be host-testable before hardware-specific backends.

## Decision 005 — Honest guarantees

Status: accepted

Soft Guard Blocks detect and manage failures.

Hard containment requires hardware.
