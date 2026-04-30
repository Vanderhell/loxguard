# Prompt 07 — Component inventory

## Task

Inspect available existing repositories/components.

Do not modify source code.

## Components to inspect

- microhealth
- microwdt
- microboot
- microassert
- microlog
- nvlog
- microtimer
- microbus
- panicdump
- microflash
- microres
- microconf
- microsh
- microcbor
- microtest
- safemath
- loxdb optionally

## For each component report

- path,
- license,
- build/test command,
- public headers,
- core API,
- heap/dependency behavior,
- loxguard relevance,
- reuse/adapt/postpone/reject decision.

## Required output

Create/update:

```text
docs/generated_component_inventory.md
```

## Do not

- Do not implement loxguard.
- Do not edit existing libraries.
- Do not refactor.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
