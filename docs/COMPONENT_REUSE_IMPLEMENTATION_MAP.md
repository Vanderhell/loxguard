# Component Reuse Map - Concrete Adapter Plan

## Principles
- Reuse if shape matches Guard Block model.
- Adapt via narrow interface when mismatch is small.
- Keep core concepts owned by loxguard.

## Planned adapter boundaries
| Adapter boundary | Purpose | Candidate |
|---|---|---|
| `lox_adapter_log_event(...)` | structured event sink | microlog |
| `lox_adapter_persist_event(...)` | optional non-volatile evidence | nvlog/microflash |
| `lox_adapter_now_ms(...)` | timeout/liveness clock source | microtimer |
| `lox_adapter_watchdog_kick(...)` | liveness cooperation | microwdt |
| `lox_adapter_panic_hook(...)` | panic/assert mapping | microassert/panicdump |
| `lox_adapter_health_set(...)` | degraded/safe mode mark | microhealth |

## No-duplication decisions
- Do not build a new logger in loxguard core.
- Do not build a new watchdog in loxguard core.
- Do not build persistence engine in MVP core.

## MVP boundary decision
- Current MVP remains adapter-agnostic and host-testable.
- Adapter API can land before real adapter implementations.
