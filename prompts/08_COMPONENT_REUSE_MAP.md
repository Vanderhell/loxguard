# Prompt 08 — Component reuse map

## Task

Map existing libraries to Guard Block product needs.

## Required output

Create/update:

```text
docs/COMPONENT_REUSE_MAP.md
```

## Map to

- Guard Block execution,
- event sink,
- blackbox storage,
- timer/timeout,
- watchdog,
- health,
- panic,
- boot-loop,
- recovery,
- debug inspection,
- serialization,
- tests,
- loxdb history.

## Decision values

```text
reuse
adapt
new loxguard-owned
postpone
reject
```

## Rule

Guard Blocks first. Existing libraries second.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
