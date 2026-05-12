# Security Policy

## Reporting a vulnerability

If you believe you have found a security issue, please open a GitHub issue with minimal details and request a private contact channel. If the repository enables private security advisories, prefer that workflow.

Include:
- a clear description of impact
- a minimal reproduction
- affected versions/commits if known

## Scope and non-goals

loxguard is a C library and does not claim:
- safety or security certification,
- protection of arbitrary C code from memory corruption,
- hardware containment without target-specific implementations and evidence.

Security reports are most actionable when tied to:
- memory safety bugs in loxguard itself,
- parsing/formatting issues in CSV/report helpers,
- denial-of-service conditions in library code.
