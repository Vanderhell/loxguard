# Event Schema (v1 core)

This document describes the event schema used by the stable host-tested core API.

Public types live in `include/loxguard.h`:

- `lox_event_t`
- `lox_event_kind_t`
- `lox_blackbox_t`

## `lox_event_t`

```c
typedef struct {
    lox_event_kind_t kind;
    const char *block_name;
    const char *reason;
    size_t index;
    size_t limit;
    uint32_t aux_code;
} lox_event_t;
```

Notes:
- `block_name` and `reason` are pointers to C strings at the time of emission. When stored into a `lox_blackbox_t`, the blackbox owns bounded copies.
- `index`/`limit` are used for bounds/arena style events.
- `aux_code` is an additional numeric field used by specific event kinds (for example diagnostic codes).

## `lox_event_kind_t` (v1)

The public enum includes:

- `LOX_EVENT_NONE`
- `LOX_EVENT_BLOCK_WRITE_OUT_OF_BOUNDS`
- `LOX_EVENT_BLOCK_ARENA_OVERFLOW`
- `LOX_EVENT_BLOCK_TIMEOUT`
- `LOX_EVENT_BLOCK_MEMORY_FAULT`
- `LOX_EVENT_BLOCK_UNSUPPORTED`
- `LOX_EVENT_BLOCK_ENTERED`
- `LOX_EVENT_BLOCK_OK`
- `LOX_EVENT_BLOCK_COMPLETED`
- `LOX_EVENT_BLOCK_ERROR`
- `LOX_EVENT_BLOCK_PANIC`
- `LOX_EVENT_BLOCK_FAULT`

See `include/loxguard.h` for the authoritative list and numeric values.

## Export/import formats

Event exports/imports are documented in:

- `docs/FORMAT_EXPORTS.md`
- `docs/API_STABILITY.md`

The export format uses percent-encoding for `block` and `reason` to keep lines reversible and parseable.
