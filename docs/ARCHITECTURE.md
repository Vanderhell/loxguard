# Architecture

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

In `v0.1.0-alpha`, the RTOS-facing API is a stub/demo interface for host testing and documentation.

### MPU backend

In `v0.1.0-alpha`, the MPU-facing API is a stub/demo interface for host testing and documentation.

## Key rule

The architecture must not start from existing libraries.

It starts from the Guard Block failure-boundary model.

Existing libraries fill roles inside that model.
