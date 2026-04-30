# Component Reuse Map

Existing libraries are accelerators.

| loxguard need | Candidate | Decision |
|---|---|---|
| health state | microhealth | reuse/adapt |
| liveness/watchdog | microwdt | reuse/adapt |
| timers | microtimer | reuse/adapt |
| structured logs | microlog | reuse/adapt |
| persistent events | nvlog | reuse/adapt |
| boot-loop | microboot | reuse/adapt |
| panic/assert | microassert | reuse/adapt |
| crash dump | panicdump | optional |
| event bus | microbus | optional |
| ring buffer | micoring | optional |
| config | microconf | optional |
| debug shell | microsh | optional |
| CBOR export | microcbor | optional |
| retry/recovery | microres | reuse/adapt |
| flash backend | microflash | optional |
| OTA rollback | microota | later |
| tests | microtest | reuse |
| safe math | safemath | internal |
| event history | loxdb | optional advanced backend |
| crypto/signing | microcrypt | later |

## Reuse policy

```text
reuse when it fits
adapt when mismatch is small
create loxguard-owned code when concept is central
postpone when not needed for MVP
reject when it adds complexity without value
```
