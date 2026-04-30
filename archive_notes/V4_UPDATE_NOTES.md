# V4 Update Notes

This package updates v3 with:

```text
Checked Guard Blocks
Software Fault Isolation direction
Memory contracts
Safe span/arena API
Instrumentation roadmap
Compiler plugin plan
Checked parser MVP
```

Important decision:

```text
LLVM/compiler plugin is not MVP.
MVP is manual checked span/arena API.
```

Main product identity is now:

```text
loxguard = Guard Blocks + Checked Guard Blocks
```

Main slogan:

```text
Wrap risky C code. Check its memory. Recover with evidence.
```
