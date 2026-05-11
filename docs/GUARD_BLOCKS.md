# Guard Blocks

## 1. Purpose

A Guard Block is a supervised execution boundary around risky C code.

It is the main developer-facing feature of loxguard.

## 2. What can be guarded

Examples:

- packet parsing,
- binary protocol decoding,
- config loading,
- OTA metadata parsing,
- customer logic,
- optional telemetry,
- network frame handling,
- non-critical analytics,
- display/UI update code,
- experimental modules.

## 3. API direction

Primary v1 API:

```c
lox_report_t loxguard_run(const loxguard_block_cfg_t *cfg,
                          loxguard_fn_t fn,
                          void *user_ctx);
```

Example:

```c
static int parse_block(lox_guard_ctx_t *g, void *ctx)
{
    parser_ctx_t *p = (parser_ctx_t *)ctx;
    /* Example: safe write through checked output span. */
    (void)g;
    return parse_packet(p->buf, p->len, &p->out);
}

void on_packet(uint8_t *buf, size_t len)
{
    parser_ctx_t ctx = { .buf = buf, .len = len };
    lox_blackbox_t bb;
    uint8_t out[256];
    uint8_t scratch[256];
    lox_blackbox_init(&bb);

    loxguard_block_cfg_t cfg = {
        .name = "packet_parser",
        .timeout_ms = 20,
        .criticality = LOXGUARD_OPTIONAL,
        .max_failures = 3,
        .blackbox = &bb,
        .input = lox_span_readonly(buf, len),
        .output = lox_span_writable(out, sizeof(out)),
        .scratch = scratch,
        .scratch_len = sizeof(scratch)
    };

    lox_report_t r = loxguard_run(&cfg, parse_block, &ctx);

    if (r.result != LOX_RESULT_OK) {
        drop_packet();
        return;
    }

    handle_packet(out);
}
```

## 4. Guard result categories

Initial result categories:

```text
OK
ERROR
TIMEOUT
PANIC
FAULT
MEMORY_FAULT
DISABLED
QUARANTINED
UNSUPPORTED
```

## 5. Honest guarantees

Soft Guard Blocks can detect and report controlled failures.

They cannot guarantee containment of arbitrary pointer corruption.

Hardware-contained Guard Blocks may provide stronger isolation where hardware supports it.

## 6. Developer value

The developer does not need to manually connect watchdog, logging, policy, and diagnostics each time.

They write:

```text
run this risky thing under loxguard
```

loxguard handles:

```text
event → policy → evidence
```
