# Prompt 04 — Guard memory contracts

## Task

Design memory contracts for Checked Guard Blocks.

## Required output

Create/update:

```text
docs/GUARD_MEMORY_CONTRACTS.md
```

## Required sections

- readable spans,
- writable spans,
- scratch arena,
- optional max stack,
- optional recursion limit,
- optional call allowlist,
- violation types,
- event mapping.

## Must include

Example parser contract:

```text
read input span
write output span
use scratch arena
```

## Do not

- Do not implement compiler instrumentation.
- Do not claim raw pointer safety without checked APIs.

## Final output format

Use exactly:
CHANGED FILES
CODE CHANGES
BUILD EVIDENCE
TEST EVIDENCE
VERIFIED FACTS
NOT VERIFIED
INCOMPLETE
