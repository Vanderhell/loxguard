# Roadmap (non-binding)

This roadmap lists possible future work items. It is not a promise or a schedule.

## v0.1.x maintenance

- Keep CI green on default + no-ecosystem configurations.
- Continue tightening documentation to match repository evidence.
- Improve test coverage for core APIs and edge cases.

## API stabilization

- Clarify which headers/APIs are intended to be stable vs. experimental.
- Add explicit API compatibility notes for future tags.

## Optional ecosystem integrations

- Add reproducible evidence (tests/logs) for specific companion-enabled builds when companion sources are available.
- Document the exact companion versions/commits used for any published evidence.

## Embedded evidence

- Add raw MCU artifacts under `artifacts/evidence/esp32/` (or other target-specific folders) for any published hardware claims.
- Document toolchain versions and build commands used to generate those artifacts.

## Backend work (design to implementation)

- RTOS backend: move from stub/demo interfaces to target-backed implementations with raw evidence.
- MPU backend: move from stub/demo interfaces to target-backed implementations with raw evidence.
