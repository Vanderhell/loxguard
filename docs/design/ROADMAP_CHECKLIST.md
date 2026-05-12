# Roadmap Checklist v3

## Phase 0 — Guard Blocks specification

- [ ] Product spec finalized.
- [ ] Guard Block API model finalized.
- [ ] Failure pipeline finalized.
- [ ] Event schema finalized.
- [ ] Policy engine finalized.
- [ ] Blackbox finalized.
- [ ] Limitations finalized.

## Phase 1 — Host MVP

- [ ] `loxguard_run()` designed.
- [ ] Event creation works.
- [ ] Basic policy works.
- [ ] RAM blackbox works.
- [ ] Host demo prints report.
- [ ] Tests prove claims.

## Phase 2 — Existing library adapters

- [ ] microtest integrated.
- [ ] microlog/nvlog adapter.
- [ ] microtimer adapter if needed.
- [ ] microhealth/microwdt adapter.
- [ ] microassert adapter.
- [ ] microboot adapter.
- [ ] microres adapter.

## Phase 3 — Embedded demo

- [ ] static/no-heap profile.
- [ ] UART report.
- [ ] MCU tick source.
- [ ] optional nvlog backend.
- [ ] bare-metal example.

## Phase 4 — RTOS backend

- [ ] FreeRTOS plan.
- [ ] task watchdog bridge.
- [ ] stack watermark event.
- [ ] task recovery demo.

## Phase 5 — MPU backend

- [ ] Cortex-M4 PoC.
- [ ] MemManage event.
- [ ] hardware evidence.
- [ ] blackbox integration.

## Phase 1.5 — Checked Guard Blocks MVP

- [ ] `lox_span_t` designed.
- [ ] `lox_arena_t` designed.
- [ ] checked read helpers.
- [ ] checked write helpers.
- [ ] checked memcpy helper.
- [ ] arena allocation helper.
- [ ] bounds violation events.
- [ ] arena violation events.
- [ ] checked parser demo.
- [ ] tests for valid/invalid reads/writes.
- [ ] tests proving invalid write is not performed.
- [ ] blackbox report includes bounds failure.
