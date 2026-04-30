# Test Evidence Plan

## Rule

Every claim must map to a test or demo.

## MVP evidence matrix

| Claim | Evidence |
|---|---|
| Guard Block can run OK code | unit test |
| Guard Block can report ERROR | unit test |
| Guard Block creates event | unit test |
| policy selects DROP_INPUT | unit test |
| blackbox stores event | unit test |
| report shows last failed block | unit test/demo |
| timeout can be represented | unit test/simulation |
| no heap by default | code inspection/build config |

## Forbidden claims without evidence

- MPU containment,
- hardware validation,
- persistent power-loss safety,
- production safety,
- memory-safe C.
