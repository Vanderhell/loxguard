# Evidence Matrix

Date: 2026-04-30  
Scope: host MVP stabilization and claim traceability

Build configs:
- `default`: `cmake -S . -B build` + `cmake --build build --config Debug` + `ctest --test-dir build -C Debug --output-on-failure`
- `nvlog`: `cmake -S . -B build_nvlog -DLOXGUARD_USE_NVLOG=ON` + `cmake --build build_nvlog --config Debug` + `ctest --test-dir build_nvlog -C Debug --output-on-failure`
- `noeco`: temporary verification with `ecosystem/` folder moved away: `cmake -S . -B build_noeco` + `cmake --build build_noeco --config Debug` + `ctest --test-dir build_noeco -C Debug --output-on-failure`

Test mapping note:
- `tests/test_pipeline.c` is a broad scenario suite (`test_pipeline_suite`) with labeled assertions (`expect(..., "label")`); claim mapping references these assertion labels.

| README claim | Test evidence | Build config | Status |
|---|---|---|---|
| Guard Block OK path is implemented | `test_pipeline_suite`: `success result is OK`, `success action NONE` | default, nvlog | VERIFIED |
| Guard Block failure path is implemented | `test_pipeline_suite`: `reason is BOUNDS`, `result is BOUNDS`, `policy action is DROP_INPUT` | default, nvlog | VERIFIED |
| Checked span OOB prevention | `test_span_suite`: `write_u8 out-of-bounds`, `span_write invalid range`, `span_memcpy dst out-of-bounds` | default, nvlog | VERIFIED |
| Arena overflow detection | `test_arena_suite`: `arena overflow returns NULL`; `test_pipeline_suite`: `arena overflow incident kind` | default, nvlog | VERIFIED |
| Success lifecycle events (`ENTERED -> OK -> COMPLETED`) | `test_pipeline_suite`: `success entered event`, `success ok event`, `success completed event` | default, nvlog | VERIFIED |
| Failure lifecycle events remain structured | `test_pipeline_suite`: `first event is ENTERED`, `incident event kind is OOB write`, `last event is COMPLETED` | default, nvlog | VERIFIED |
| Policy decision pipeline | `test_pipeline_suite`: `policy reset-block for non-failure kind`, `policy action is DROP_INPUT` | default, nvlog | VERIFIED |
| Blackbox/report incident evidence | `test_pipeline_suite`: blackbox rollover/ownership checks, report field checks | default, nvlog | VERIFIED |
| Report export/import parser robustness | `test_pipeline_suite`: `report parse line`, `report parse ex line`, invalid action/event/persisted rejections | default, nvlog | VERIFIED |
| CSV event export/import parser robustness | `test_pipeline_suite`: format/parse round-trip, mixed valid/invalid import, trailing-garbage rejection | default, nvlog | VERIFIED |
| safemath integration in checked arithmetic | Build-time integration in `CMakeLists.txt` (`LOXGUARD_HAVE_SAFEMATH`), runtime bounds tests in `test_span_suite`/`test_arena_suite` | default, nvlog | PARTIAL |
| microlog integration in adapter logging | Build-time integration in `CMakeLists.txt` (`mlog.c`), adapter path exercised by pipeline flows | default, nvlog | PARTIAL |
| microhealth partial integration | `test_pipeline_suite`: `health state OK after successful block`, `health state not-OK after ...` | default, nvlog | PARTIAL |
| nvlog host persistence integration | `test_pipeline_suite`: `nvlog init ram`, `persist success on nvlog`, `persist failure on injected nvlog fault`, `report persisted true with nvlog initialized` | nvlog | VERIFIED |
| ecosystem companion layout is canonical | `CMakeLists.txt` ecosystem-first path with legacy fallback; optional-free build passes when `ecosystem/` is absent | default, nvlog, noeco | VERIFIED |
| RTOS backend production behavior | Stub-only tests (`test_ports_suite`, `test_pipeline_suite`) | default, nvlog | NOT VERIFIED |
| MPU backend production behavior | Stub-only tests (`test_ports_suite`, `test_pipeline_suite`) | default, nvlog | NOT VERIFIED |
| LLVM/plugin instrumentation mode | no runtime tests in host MVP | default, nvlog | NOT VERIFIED |
