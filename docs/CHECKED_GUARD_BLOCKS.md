# Checked Guard Blocks

## 1. Purpose

Checked Guard Blocks are Guard Blocks with explicit memory contracts.

They are the bridge between:

```text
portable soft failure handling
```

and

```text
hardware MPU containment
```

They allow loxguard to detect many invalid memory accesses before corruption happens, even on MCUs without MPU.

## 2. Core idea

Developer defines what the block may access:

```text
readable input spans
writable output spans
scratch arena
optional stack budget
optional recursion budget
optional allowed calls/capabilities
```

The guarded code then uses checked helpers.

Example:

```c
uint8_t b;
if (lox_span_read_u8(ctx->input, i, &b) != LOXGUARD_OK) {
    return LOXGUARD_ERR_BOUNDS;
}

if (lox_span_write_u8(ctx->output, out_i, b) != LOXGUARD_OK) {
    return LOXGUARD_ERR_BOUNDS;
}
```

## 3. What this catches

Checked Guard Blocks can catch:

- out-of-bounds reads,
- out-of-bounds writes,
- invalid copy sizes,
- scratch arena overflow,
- null span use,
- integer overflow in checked sizing when using safemath,
- optional recursion depth violations,
- optional forbidden allocation/call violations.

## 4. What this does not catch

Without compiler instrumentation or hardware isolation, Checked Guard Blocks cannot catch every raw pointer access.

Example unsafe raw pointer:

```c
uint8_t *p = unknown_pointer();
*p = 42;
```

If guarded code uses raw pointer writes directly, loxguard cannot automatically check it unless instrumentation is enabled.

## 5. Correct claim

```text
Checked Guard Blocks prevent many common buffer and arena mistakes when guarded code uses loxguard's checked memory API.
```

## 6. Incorrect claim

```text
Checked Guard Blocks make arbitrary C memory-safe.
```

## 7. MVP scope

MVP should implement:

- `lox_span_t`,
- `lox_arena_t`,
- checked read/write helpers,
- checked memcpy/memset helpers,
- bounds violation event,
- blackbox evidence,
- parser demo.

MVP should not implement:

- LLVM plugin,
- automatic pointer analysis,
- MPU,
- dynamic loading.

## 8. Demo

Expected demo:

```text
[guard] enter packet_parser
[check] output write out of bounds index=17 len=16
[event] BLOCK_BOUNDS_VIOLATION source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser reason=BOUNDS action=DROP_INPUT
```
