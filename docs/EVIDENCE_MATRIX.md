# Evidence Matrix

Date: 2026-05-01  
Scope: host MVP stabilization, hardware bench validation, and claim traceability

Raw host logs (when checked in) live under:

`artifacts/evidence/host/`

Build configs:
- `default`: `cmake -S . -B build` + `cmake --build build --config Debug` + `ctest --test-dir build -C Debug --output-on-failure`
- `nvlog`: `cmake -S . -B build_nvlog -DLOXGUARD_USE_NVLOG=ON` + `cmake --build build_nvlog --config Debug` + `ctest --test-dir build_nvlog -C Debug --output-on-failure`
- `noeco`: temporary verification with `ecosystem/` folder moved away: `cmake -S . -B build_noeco` + `cmake --build build_noeco --config Debug` + `ctest --test-dir build_noeco -C Debug --output-on-failure`
- `microtimer`: `cmake -S . -B build_mtimer -DLOXGUARD_USE_MICROTIMER=ON` + build + test
- `microwdt`: `cmake -S . -B build_mwdt -DLOXGUARD_USE_MICROWDT=ON` + build + test
- `microtimer_microwdt`: `cmake -S . -B build_mtimer_mwdt -DLOXGUARD_USE_MICROTIMER=ON -DLOXGUARD_USE_MICROWDT=ON` + build + test
- `microres`: `cmake -S . -B build_mres -DLOXGUARD_USE_MICRORES=ON` + build + test
- `microres_microtimer_microwdt`: `cmake -S . -B build_mres_mtimer_mwdt -DLOXGUARD_USE_MICRORES=ON -DLOXGUARD_USE_MICROTIMER=ON -DLOXGUARD_USE_MICROWDT=ON` + build + test

Test mapping note:
- `tests/test_pipeline.c` is a broad scenario suite (`test_pipeline_suite`) with labeled assertions (`expect(..., "label")`); claim mapping references these assertion labels.

CI note:
- GitHub Actions CI verifies `default` and `no-ecosystem` only.
- Companion-enabled configs are validated locally when companion sources are present under `ecosystem/`.

| README claim | Test evidence | Build config | Status |
|---|---|---|---|
| Guard Block OK path is implemented | `test_pipeline_suite`: `success result is OK`, `success action NONE` | default, nvlog | VERIFIED |
| Guard Block failure path is implemented | `test_pipeline_suite`: `reason is BOUNDS`, `result is BOUNDS`, `policy action is DROP_INPUT` | default, nvlog | VERIFIED |
| Controlled panic/fault evidence path is implemented | `test_pipeline_suite`: `panic demo ...`, `fault demo ...` assertions (reason/result/action/incident kind) | default, microtimer, microwdt, microtimer_microwdt | VERIFIED |
| Checked span OOB prevention | `test_span_suite`: `write_u8 out-of-bounds`, `span_write invalid range`, `span_memcpy dst out-of-bounds` | default, nvlog | VERIFIED |
| Checked span edge overflow guards | `test_span_suite`: `span_read overflowed offset rejected`, zero-length boundary reads/writes, null argument guards | default, nvlog | VERIFIED |
| Arena overflow detection | `test_arena_suite`: `arena overflow returns NULL`; `test_pipeline_suite`: `arena overflow incident kind` | default, nvlog | VERIFIED |
| Arena overflow/overflow-arithmetic guards | `test_arena_suite`: `arena huge size overflow returns NULL`, corrupted `used` state rejection | default, nvlog | VERIFIED |
| Success lifecycle events (`ENTERED -> OK -> COMPLETED`) | `test_pipeline_suite`: `success entered event`, `success ok event`, `success completed event` | default, nvlog | VERIFIED |
| Failure lifecycle events remain structured | `test_pipeline_suite`: `first event is ENTERED`, `incident event kind is OOB write`, `last event is COMPLETED` | default, nvlog | VERIFIED |
| Policy decision pipeline | `test_pipeline_suite`: `policy none for non-failure kind`, `policy action is DROP_INPUT` | default, nvlog | VERIFIED |
| Blackbox/report incident evidence | `test_pipeline_suite`: blackbox rollover/ownership checks, report field checks | default, nvlog | VERIFIED |
| Report export/import parser robustness | `test_pipeline_suite`: `report parse line`, `report parse ex line`, invalid action/event/persisted rejections | default, nvlog | VERIFIED |
| CSV event export/import parser robustness | `test_pipeline_suite`: format/parse round-trip, mixed valid/invalid import, trailing-garbage rejection | default, nvlog | VERIFIED |
| CSV/report malformed/trailing-token rejection hardening | `test_pipeline_suite`: trailing-whitespace rejection, empty numeric token rejection, mutation-loop invalid CSV rejects | default, nvlog | VERIFIED |
| safemath integration in checked arithmetic | Build-time integration in `CMakeLists.txt` (`LOXGUARD_HAVE_SAFEMATH`), runtime bounds tests in `test_span_suite`/`test_arena_suite` | default, nvlog | PARTIAL |
| microlog integration in adapter logging | Build-time integration in `CMakeLists.txt` (`mlog.c`), adapter path exercised by pipeline flows | default, nvlog | PARTIAL |
| microhealth partial integration | `test_pipeline_suite`: `health state OK after successful block`, `health state not-OK after ...` | default, nvlog | PARTIAL |
| microtimer optional integration | build/test pass in `build_mtimer`; deterministic injected time invariants still pass in `test_pipeline_suite` | microtimer | PARTIAL |
| microwdt optional integration | build/test pass in `build_mwdt`; `test_pipeline_suite` watchdog state mappings for success/failure/timeout | microwdt | PARTIAL |
| combined microtimer+microwdt integration | build/test pass in `build_mtimer_mwdt` | microtimer_microwdt | PARTIAL |
| microres optional recovery/circuit-breaker integration | `test_pipeline_suite`: repeated failure increments breaker; breaker-open blocks later attempt (`BREAKER_OPEN`) | microres | PARTIAL |
| combined microres+microtimer+microwdt integration | build/test pass in `build_mres_mtimer_mwdt` | microres_microtimer_microwdt | PARTIAL |
| watchdog late/starved semantics representation | `test_pipeline_suite` watchdog non-OK mapping checks; semantics represented via adapter state | default, microtimer, microwdt, microtimer_microwdt | PARTIAL |
| microassert companion integration | optional switch + companion-header detection in CMake; module unavailable in workspace | default | NOT VERIFIED |
| panicdump companion integration | optional switch + companion-header detection in CMake; module unavailable in workspace | default | NOT VERIFIED |
| nvlog host persistence integration | `test_pipeline_suite`: `nvlog init ram`, `persist success on nvlog`, `persist failure on injected nvlog fault`, `report persisted true with nvlog initialized` | nvlog | VERIFIED |
| nvlog failure-injection/init guard behavior | `test_pipeline_suite`: undersized init rejection, null-path rejection, post-shutdown unsupported persist | nvlog | VERIFIED |
| Blackbox long-run rollover stability | `test_pipeline_suite`: `blackbox long stress ...` checks after `2020` inserts | default, nvlog | VERIFIED |
| ecosystem companion layout is canonical | `CMakeLists.txt` ecosystem-first path with legacy fallback; optional-free build passes when `ecosystem/` is absent | default, nvlog, noeco | VERIFIED |
| RTOS backend production behavior | Stub-only tests (`test_ports_suite`, `test_pipeline_suite`) | default, nvlog | NOT VERIFIED |
| MPU backend production behavior | Stub-only tests (`test_ports_suite`, `test_pipeline_suite`) | default, nvlog | NOT VERIFIED |
| LLVM/plugin instrumentation mode | no runtime tests in host MVP | default, nvlog | NOT VERIFIED |

## Hardware bench evidence (ESP32-S3) — external notes

This section is a human-maintained bench summary. Raw logs/artifacts are not included in the repo by default.

To make these claims independently reviewable, add artifacts under:

`artifacts/evidence/esp32/`

Until raw artifacts are checked in, treat all ESP32-S3 bullet points in this section as **NOT VERIFIED from this repo**.

- Date: 2026-05-01
- Board run evidence:
  - `[OK] SD mounted card=7580MB`
  - smoke and stress phases completed
  - per-case pass summary:
    - checked_ok 500/500
    - checked_oob 500/500
    - arena_overflow 500/500
    - timeout_demo 250/250
    - panic_demo 250/250
    - fault_demo 250/250
    - rtos_host 250/250
    - rtos_stub 250/250
    - mpu_host 250/250
    - mpu_stub 250/250
  - total summary: `pass=3260/3260 fail=0`
- Status impact:
  - confirms guarded pipeline behavior on real target runtime for bench scenarios
  - does not yet replace full production RTOS/MPU backend verification

## MCU RAM/flash footprint (ESP32-S3 Arduino build) — external notes

- Date: 2026-05-01
- Target: `esp32:esp32:esp32s3` (ESP32S3 Dev Module)
- Compile command:
  - `arduino-cli compile --fqbn esp32:esp32:esp32s3 examples/bench/loxguard_bench`
- Reported sizes:
  - Flash: `Sketch uses 440988 bytes (33%)` of `1310720` bytes.
  - RAM: `Global variables use 27368 bytes (8%)`, leaving `300312` bytes (max `327680` bytes).
- Status impact:
  - provides real MCU toolchain footprint numbers for the bench firmware (default build).
  - provides a baseline for comparing companion-heavy (LCD) vs minimal builds.

### Minimal build (LCD disabled)

- Compile command:
  - `arduino-cli compile --fqbn esp32:esp32:esp32s3 --build-property compiler.cpp.extra_flags='-DLOX_BENCH_LCD_ENABLE=0' --build-property compiler.c.extra_flags='-DLOX_BENCH_LCD_ENABLE=0' examples/bench/loxguard_bench`
- Reported sizes:
  - Flash: `Sketch uses 378247 bytes (28%)` of `1310720` bytes.
  - RAM: `Global variables use 23604 bytes (7%)`, leaving `304076` bytes (max `327680` bytes).

## SD log tail dump (on-device evidence assist) — external notes

- Date: 2026-05-01
- Firmware supports serial command `d` to print the tail of `/loxguard_bench.log` from SD (`[sd] --- log tail ---` ... `[sd] --- end ---`).
- Verified behavior:
  - tail contains expected `=== loxguard bench boot ===` separators and scenario lines after boot.
  - enables collecting persistence evidence without removing the SD card.

## Release footprint snapshots (host toolchain artifacts) — external notes

- Date: 2026-05-01
- Build commands:
  - `cmake -S . -B build_rel`
  - `cmake --build .\build_rel --config Release`
  - `ctest --test-dir .\build_rel -C Release --output-on-failure`
  - `cmake -S . -B build_nvlog_rel -DLOXGUARD_USE_NVLOG=ON`
  - `cmake --build .\build_nvlog_rel --config Release`
  - `ctest --test-dir .\build_nvlog_rel -C Release --output-on-failure`
- Sizes:
  - default `build_rel/Release/loxguard.lib`: `117848` bytes
  - default `build_rel/Release/loxguard_demo.exe`: `23552` bytes
  - nvlog `build_nvlog_rel/Release/loxguard.lib`: `143900` bytes
  - nvlog `build_nvlog_rel/Release/loxguard_demo.exe`: `26112` bytes
- Status impact:
  - baseline size delta from enabling nvlog profile is measured
  - MCU flash/ram consumption must still be collected from target toolchain output

## Power-loss and restart consistency evidence (executed) — external notes

- Date: 2026-05-01
- Procedure:
  - started soak via serial `s`,
  - cut power abruptly during active soak after checkpoints,
  - rebooted board and verified full bench startup run,
  - repeated for at least 5 cycles.
- Observed checkpoints before forced cuts (examples): `2000`, `3000`, `5000`, `9000`, `10000`.
- Post-restart behavior:
  - board consistently mounted SD and resumed normal benchmark startup,
  - each post-restart benchmark run completed with `pass=3260/3260 fail=0`.
- Log integrity evidence from `/loxguard_bench.log`:
  - valid `soak_checkpoint` lines persisted (e.g. loops `7000`..`10000`),
  - `=== loxguard bench boot ===` separators present after reboot,
  - provided tail shows coherent lines without truncation artifacts.
- Evidence extraction method:
  - serial command `d` prints an SD log tail window to serial, confirming last `soak_checkpoint` before reboot (examples observed via `d`: `loops=3000`, `loops=5000`, `loops=7000`).
- Status impact:
  - host/bench persistence path demonstrates restart consistency under abrupt power cuts on tested hardware setup.
  - this validates current MVP persistence behavior for the exercised path; production flash-specific guarantees remain platform-dependent.
