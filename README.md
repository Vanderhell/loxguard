# loxguard

Host-tested MVP for Guard Blocks + Checked primitives + evidence pipeline.

```text
Wrap risky C code. Check its memory. Recover with evidence.
```

## Status Snapshot

Verified now:
- Host Guard Block lifecycle (success + failure paths).
- Checked span/arena safety checks.
- Policy + blackbox + report pipeline.
- CSV/report evidence export/import.
- Optional companion integrations:
  - `safemath` (checked arithmetic path),
  - `microlog` (logging adapter path),
  - partial `microhealth` mapping,
  - host `nvlog` persistence (`LOXGUARD_USE_NVLOG=ON`).

Not verified now:
- Production RTOS backend behavior.
- Production MPU backend behavior.
- LLVM/plugin instrumentation mode.
- Production embedded flash/EEPROM/FRAM persistence behavior.

## Quick Example

```c
#include "loxguard.h"

int main(void) {
    uint8_t in[8] = {0,1,2,3,4,5,6,7};
    uint8_t out[16] = {0};
    uint8_t scratch[64] = {0};
    lox_blackbox_t bb;
    lox_report_t report;

    report = lox_run_checked_parser_demo(
        in, sizeof(in),
        out, sizeof(out),
        scratch, sizeof(scratch),
        &bb
    );

    return (report.result == LOX_RESULT_OK) ? 0 : 1;
}
```

## Core concept

Developers wrap risky code with a Guard Block.

For basic use, the block can report errors, timeouts, panic/assert failures, and recovery failures.

For checked use, the block receives explicit memory contracts:

```text
readable input spans
writable output spans
scratch arenas
stack/recursion budgets
allowed calls/capabilities
```

Then loxguard can catch many mistakes before they corrupt firmware state.

## Important distinction

loxguard has levels:

```text
Level 1: Soft Guard Blocks
  detect errors, timeout, panic, failure policy, blackbox evidence

Level 2: Checked Guard Blocks
  checked spans, checked writes, scratch arena, bounds events

Level 3: Instrumented Guard Blocks
  future compiler/LLVM-assisted checks

Level 4: RTOS Guard Blocks
  task/job supervision

Level 5: MPU Guard Blocks
  hardware memory containment where available
```

## MVP direction

Do not start with LLVM/plugin.

Do not start with MPU.

The first stronger MVP is:

```text
packet_parser Guard Block
+ input span
+ output span
+ scratch arena
+ checked read/write helpers
+ bounds violation event
+ policy action
+ blackbox report
```

Expected demo:

```text
[guard] enter packet_parser
[check] output write out of bounds index=17 len=16
[event] BLOCK_BOUNDS_VIOLATION source=packet_parser
[policy] action=DROP_INPUT
[blackbox] stored event
[report] last_failed_block=packet_parser reason=BOUNDS action=DROP_INPUT
```

## Recommended Codex sequence

Use the prompts in this order:

```text
00_MASTER_CONTEXT.md
01_PRODUCT_SPEC_GUARD_BLOCKS.md
02_GUARD_BLOCK_API_DESIGN.md
03_CHECKED_GUARD_BLOCKS.md
04_MEMORY_CONTRACTS.md
05_SAFE_SPAN_ARENA_API.md
06_FAILURE_PIPELINE_DESIGN.md
07_EVENT_SCHEMA.md
08_POLICY_ENGINE.md
09_BLACKBOX.md
10_COMPONENT_INVENTORY.md
11_COMPONENT_REUSE_MAP.md
12_PROFILES.md
13_MVP_CHECKED_PARSER_DEMO_DESIGN.md
14_MVP_IMPLEMENTATION_PLAN.md
15_TEST_EVIDENCE_PLAN.md
16_INSTRUMENTATION_ROADMAP.md
17_COMPILER_PLUGIN_PLAN.md
18_RTOS_BACKEND_PLAN.md
19_MPU_BACKEND_PLAN.md
20_REVIEW_AND_CONSOLIDATE.md
```

## Required Codex output format

Every task must end with:

```text
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
```

## Project truth

- Soft Guard Blocks do not make C memory-safe.
- Checked Guard Blocks require explicit memory contracts.
- Compiler instrumentation is future work, not MVP.
- MPU containment is optional and hardware-dependent.
- Existing libraries are accelerators, not product boundaries.

## v0.1 Host MVP Freeze

Current branch is in `v0.1 host MVP feature freeze`.

No new product features are in scope for this phase.
Only:
- regression tests,
- cleanup,
- evidence documentation,
- README truth alignment.

## Companion Ecosystem Layout

- Canonical folder: `ecosystem/`
- Libraries are companion ecosystem modules (sibling repositories), not foreign dependencies.
- Legacy fallback is supported for folder name `third_party/` during migration only.

## Verified Capabilities (Host MVP)

- Host Guard Blocks flow (detect -> classify -> policy -> action -> evidence -> report).
- Checked span/arena paths for parser demo.
- Policy actions:
  - failure events -> `DROP_INPUT`,
  - non-failure kind -> `RESET_BLOCK`.
- Blackbox bounded rollover with owned stable event text (`block_name`, `reason`).
- Evidence serialization/import:
  - event CSV export/import,
  - report KV export/import.
- Parser robustness:
  - enum range validation,
  - oversized token rejection,
  - trailing-garbage rejection,
  - mixed valid/invalid import behavior.
- Ecosystem reuse in current code:
  - `safemath` used for overflow-safe span/arena arithmetic,
  - `microlog` integrated in adapter logging path,
  - `microhealth` adapter-level state mapping integrated (partial),
  - adapter timing contract exposed for `microtimer` style clock sources (unified duration path),
  - `nvlog` host persistence adapter available (optional, `LOXGUARD_USE_NVLOG=ON`).

## Capability Matrix

| Capability | Status | Evidence | Notes |
|---|---|---|---|
| Guard Block OK path | VERIFIED | `tests/test_pipeline.c` (`success result is OK`, lifecycle checks) | host MVP |
| Guard Block failure path | VERIFIED | `tests/test_pipeline.c` (`reason is BOUNDS`, action checks) | host MVP |
| Checked span OOB prevention | VERIFIED | `tests/test_span.c` + pipeline OOB checks | pre-write rejection path |
| Arena overflow detection | VERIFIED | `tests/test_arena.c` + `tests/test_pipeline.c` | deterministic overflow handling |
| Success lifecycle events | VERIFIED | `tests/test_pipeline.c` (`ENTERED`, `OK`, `COMPLETED`) | explicit success evidence |
| Failure lifecycle events | VERIFIED | `tests/test_pipeline.c` incident + completion flow | explicit failure evidence |
| Policy decisions | VERIFIED | `tests/test_pipeline.c` policy assertions | `DROP_INPUT` / `RESET_BLOCK` |
| Blackbox/report evidence | VERIFIED | `tests/test_pipeline.c` ownership/rollover/report checks | bounded local evidence |
| CSV event export/import | VERIFIED | `tests/test_pipeline.c` round-trip + invalid-line rejection | parser hardening included |
| Report export/import | VERIFIED | `tests/test_pipeline.c` kv round-trip + negative cases | strict schema checks |
| safemath integration | PARTIAL | build integration + span/arena boundary tests | direct safemath call path not isolated by dedicated unit test |
| microlog integration | PARTIAL | build integration + adapter path execution in pipeline tests | log sink side effects not asserted |
| microhealth integration | PARTIAL | `tests/test_pipeline.c` health state assertions | adapter-level mapping only |
| nvlog host persistence | VERIFIED | `tests/test_pipeline.c` nvlog init/success/failure + persisted report flag | requires `LOXGUARD_USE_NVLOG=ON` |
| RTOS production backend | NOT VERIFIED | stub-only tests | real RTOS not in MVP |
| MPU production backend | NOT VERIFIED | stub-only tests | real hardware not in MVP |
| LLVM/plugin instrumentation | NOT VERIFIED | no host MVP runtime coverage | future scope |

## Not Verified

- Production RTOS backend behavior on target RTOS.
- Production MPU backend behavior on real hardware.
- Hardware timing/fault behavior outside host/stub models.
- LLVM/compiler instrumentation mode.
- OTA/module signing/cloud/DB backends.
- `microwdt`, `microtest`, `microbus`, `microres`, `microsh`, `loxdb` runtime integration in loxguard.
- production-grade embedded flash/EEPROM/FRAM persistence behavior for `nvlog` adapter path.

## nvlog Host Persistence (Optional)

- Enable with CMake option: `-DLOXGUARD_USE_NVLOG=ON`
- Companion library discovery uses canonical `ecosystem/` first and legacy `third_party/` fallback.
- Current verified adapter scope:
  - host RAM backend (`lox_adapter_nvlog_init_ram(...)`)
  - host file backend (`lox_adapter_nvlog_init_file(...)`)
- If adapter is not initialized, `lox_adapter_persist_event(...)` cleanly falls back to unsupported state.

## Verified Build Configurations

Default:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

nvlog-enabled:

```powershell
cmake -S . -B build_nvlog -DLOXGUARD_USE_NVLOG=ON
cmake --build build_nvlog --config Debug
ctest --test-dir build_nvlog -C Debug --output-on-failure
```

## Claim -> Test -> Status

| Claim | Test | Status |
|---|---|---|
| OOB write is detected before write | `test_pipeline_suite` (`buffer tail unchanged`, OOB event checks) | VERIFIED |
| Span read/write and memcpy bounds are enforced | `test_span_suite` | VERIFIED |
| Arena overflow is detected deterministically | `test_arena_suite`, `test_pipeline_suite` | VERIFIED |
| Failure events map to `DROP_INPUT` policy | `test_pipeline_suite` | VERIFIED |
| Non-failure kind maps to `RESET_BLOCK` policy | `test_pipeline_suite` (`policy reset-block for non-failure kind`) | VERIFIED |
| Blackbox keeps stable owned strings | `test_pipeline_suite` (`ownership_bb`, `rollover_bb`) | VERIFIED |
| Blackbox rollover keeps bounded tail window | `test_pipeline_suite` (`20 -> 16` rollover checks) | VERIFIED |
| CSV export/import round-trip works | `test_pipeline_suite` (`dump_h` import checks) | VERIFIED |
| CSV import skips invalid lines and keeps valid order | `test_pipeline_suite` (`mixed_import`) | VERIFIED |
| CSV/report parsers reject trailing garbage | `test_pipeline_suite` | VERIFIED |
| CSV/report parsers reject invalid enum/range | `test_pipeline_suite` | VERIFIED |
| CSV/report parsers reject oversized tokens | `test_pipeline_suite` | VERIFIED |

## Evidence Documents

- `docs/EVIDENCE_MATRIX.md`
- `docs/COMPONENT_INTEGRATION_STATUS.md`
- `docs/TEST_EVIDENCE_PLAN.md`
- `CHANGELOG.md`
- `docs/RELEASE_NOTES_v0.1.0-alpha.md`
