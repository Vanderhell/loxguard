# Codex Review Prompt — loxguard Guard Blocks v4

Review the repository against the v4 direction.

Do not modify files.

## Check

- Is loxguard presented as Guard Blocks + Checked Guard Blocks for embedded C?
- Is the slogan consistent: "Wrap risky C code. Check its memory. Recover with evidence."?
- Is the failure pipeline underneath Guard Blocks?
- Are memory contracts explicit?
- Is span/arena API the MVP path?
- Is compiler instrumentation future-only?
- Are existing libraries accelerators, not boundaries?
- Is MPU optional?
- Is RTOS optional?
- Is the host checked parser MVP first?
- Are Soft Guard Block limitations honest?
- Are Checked Guard Block limitations honest?
- Are memory-safety claims avoided?
- Are tests/evidence required for claims?
- Is scope creep present?

## Output format

Use exactly:

```text
REVIEW SUMMARY
CORRECT
INCORRECT
DUPLICATION RISKS
UNSUPPORTED CLAIMS
SCOPE CREEP
MISSING EVIDENCE
REQUIRED FIXES
OPTIONAL IMPROVEMENTS
```

Be strict. Do not praise unverified claims.
