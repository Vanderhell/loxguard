# loxguard Architecture v3

## High-level model

```text
Application firmware
        |
        v
Guard Blocks / Module API
        |
        v
Failure Pipeline
        |
        +--> Event Schema
        +--> Policy Engine
        +--> Blackbox
        +--> Action Dispatcher
        |
        v
Adapters to existing libraries
        |
        v
Optional backends: RTOS / MPU / loxdb
```

## loxguard-core owns

- Guard Block API,
- source/block registry,
- event creation,
- policy evaluation,
- action dispatch,
- mode state.

## loxguard-blackbox owns

- last failure snapshot,
- recent event ring,
- text report,
- optional persistence bridge.

## loxguard-adapters bridge

- microhealth,
- microwdt,
- microboot,
- microassert,
- panicdump,
- microtimer,
- microbus,
- microlog,
- nvlog,
- microres,
- microconf,
- microsh,
- microcbor,
- microflash,
- loxdb optional.

## Optional backends

### RTOS backend

Maps Guard Blocks to supervised tasks/jobs.

### MPU backend

Maps Guard Blocks to hardware-contained regions where supported.

## Key rule

The architecture must not start from existing libraries.

It starts from the Guard Block failure-boundary model.

Existing libraries fill roles inside that model.
