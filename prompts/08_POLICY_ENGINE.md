# v4 alias/update

This prompt supersedes or updates `05_POLICY_ENGINE.md` in the v4 order.

# Prompt 05 — Policy engine

## Task

Design deterministic policy engine for Guard Block failures.

## Required output

Create/update:

```text
docs/POLICY_ENGINE.md
```

## Must map

```text
event + source config + history + criticality -> action
```

## Required actions

- DROP_INPUT,
- RECOVER_BLOCK,
- RECOVER_MODULE,
- DISABLE_MODULE,
- QUARANTINE_MODULE,
- ENTER_DEGRADED_MODE,
- ENTER_SAFE_MODE,
- STORE_BLACKBOX.

## Constraints

- deterministic,
- bounded,
- static by default,
- no heap,
- host-testable.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
