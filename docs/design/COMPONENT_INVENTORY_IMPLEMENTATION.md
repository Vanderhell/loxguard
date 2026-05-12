# Component Inventory - Implementation Facing

## Scope (current repo)
- Implemented core:
  - Guard context + checked spans/arena
  - Event -> policy -> blackbox -> report pipeline
  - Host demo + host tests
- Not yet implemented:
  - External adapter code for micro* libraries
  - RTOS backend
  - MPU backend
  - Instrumentation backend

## Inventory table
| Need | Current owner | Candidate reuse | Current decision |
|---|---|---|---|
| Checked span ops | loxguard core | safemath (optional later) | keep in loxguard (central) |
| Scratch arena | loxguard core | none required | keep in loxguard (central) |
| Policy decision | loxguard core | microres (later integration) | keep local now |
| Volatile blackbox ring | loxguard core | micoring (optional) | keep local now |
| Log sink | none yet | microlog | add adapter later |
| Persistent event sink | none yet | nvlog/microflash | add adapter later |
| Panic/assert bridge | none yet | microassert/panicdump | add adapter later |
| Timing/timeout source | none yet | microtimer/microwdt | add adapter later |
| Health/degraded state | none yet | microhealth | add adapter later |

## Immediate follow-up implementation tasks
1. Add adapter interfaces in `include/loxguard_adapters.h` (no external deps).
2. Add stub adapter implementations in `src/loxguard_adapters.c`.
3. Add tests proving core works without any adapter linked.
