# loxguard Unified Product Specification v4

## 1. Definition

`loxguard` gives embedded C developers **Guard Blocks** and **Checked Guard Blocks**.

A Guard Block is a supervised failure boundary around risky firmware code.

A Checked Guard Block adds explicit memory contracts so many buffer/pointer mistakes can be detected before they corrupt firmware state.

Short definition:

```text
Wrap risky C code. Check its memory. Recover with evidence.
```

## 2. Product identity

loxguard is not only:

- a watchdog,
- a logger,
- a health monitor,
- an MPU sandbox,
- a compiler sanitizer,
- an integration layer.

loxguard is:

```text
Guard Blocks for embedded C
+ Checked Guard Blocks
+ local failure-management pipeline
+ deterministic recovery policy
+ local blackbox evidence
+ optional toolchain/hardware containment
```

## 3. Main user value

A developer can mark risky C code as a controlled failure boundary.

Examples:

- packet parser,
- protocol handler,
- application logic,
- optional telemetry,
- config parser,
- OTA metadata parser,
- companion ecosystem module,
- experimental feature.

Without loxguard:

```text
risky code fails → mystery crash / watchdog reset / corrupted state / device dead
```

With loxguard:

```text
risky block fails → block failure event → policy action → blackbox evidence
```

With Checked Guard Blocks:

```text
risky block tries invalid read/write → bounds violation event before write → policy action → evidence
```

## 4. Guard Block levels

### Level 1 — Soft Guard Blocks

Portable.

Handles:

- return-code errors,
- controlled fail calls,
- panic/assert integration,
- timeout/liveness,
- policy,
- blackbox evidence.

Does not provide arbitrary memory-corruption containment.

### Level 2 — Checked Guard Blocks

Portable, contract-based.

Adds:

- input spans,
- output spans,
- scratch arenas,
- checked reads,
- checked writes,
- checked copy/set helpers,
- bounds violation events,
- arena overflow events.

This is the first strong MVP target.

### Level 3 — Instrumented Guard Blocks

Future toolchain-assisted mode.

Possible techniques:

- Clang/LLVM pass,
- source rewriting,
- macro-assisted wrappers,
- static analysis,
- call allowlists,
- automatic access reports.

Not MVP.

### Level 4 — RTOS Guard Blocks

Task/job supervision for systems with an RTOS.

### Level 5 — MPU Guard Blocks

Hardware containment where supported.

## 5. Preferred v1 API

Function-based first:

```c
typedef int (*loxguard_fn_t)(void *ctx);

typedef struct loxguard_block_cfg {
    const char *name;
    uint32_t timeout_ms;
    uint8_t criticality;
    uint8_t max_failures;
} loxguard_block_cfg_t;

int loxguard_run(const loxguard_block_cfg_t *cfg,
                 loxguard_fn_t fn,
                 void *ctx);
```

Checked block context:

```c
typedef struct loxguard_contract {
    lox_span_t *reads;
    uint16_t read_count;
    lox_span_t *writes;
    uint16_t write_count;
    lox_arena_t *scratch;
} loxguard_contract_t;
```

## 6. Failure pipeline

```text
detect → classify → decide → act → persist → inspect
```

Guard Blocks are the developer-facing entry point.

The pipeline is the system underneath.

## 7. Existing libraries

Existing libraries are accelerators:

- microhealth,
- microwdt,
- microboot,
- microassert,
- microlog,
- nvlog,
- microtimer,
- microbus,
- panicdump,
- microres,
- microconf,
- microsh,
- microcbor,
- microflash,
- microtest,
- safemath,
- loxdb optional.

They should be reused where they fit, but the Guard Block model comes first.

## 8. MVP

First strong MVP:

```text
Checked parser Guard Block
```

Scenario:

```text
packet_parser receives input span and output span
parser tries out-of-bounds output write
loxguard detects bounds violation before write
event created
policy selects DROP_INPUT
blackbox stores evidence
report printed
```

MVP does not require:

- MCU,
- MPU,
- RTOS,
- cloud,
- OTA,
- signed modules,
- compiler plugin.

## 9. Product non-claims

loxguard does not claim:

- memory-safe C everywhere,
- protection against all undefined behavior,
- protection against DMA bugs,
- production certification,
- cloud observability,
- Docker-like containers,
- universal MCU isolation,
- automatic compiler instrumentation in MVP.

## 10. Final product statement

```text
loxguard gives embedded C firmware explicit Guard Blocks.
Checked Guard Blocks add memory contracts and checked access helpers.
When risky code fails, loxguard records what happened, decides what to do, and keeps local evidence for inspection.
```
