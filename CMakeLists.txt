cmake_minimum_required(VERSION 3.16)
project(loxguard C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
option(LOXGUARD_USE_NVLOG "Enable nvlog host/RAM persistence adapter" OFF)
option(LOXGUARD_USE_MICROTIMER "Enable microtimer companion timing adapter path" OFF)
option(LOXGUARD_USE_MICROWDT "Enable microwdt companion watchdog/liveness adapter path" OFF)

add_library(loxguard STATIC
    src/loxguard.c
    src/loxguard_adapters.c
    src/adapters/loxguard_nvlog_adapter.c
    src/loxguard_backends.c
    src/loxguard_format.c
    src/loxguard_rtos_bridge.c
    src/loxguard_ports.c
)

target_include_directories(loxguard PUBLIC include)

set(LOX_ECOSYSTEM_DIR "${CMAKE_SOURCE_DIR}/ecosystem")
if(NOT EXISTS "${LOX_ECOSYSTEM_DIR}")
    set(LOX_ECOSYSTEM_DIR "${CMAKE_SOURCE_DIR}/third_party")
endif()

if(EXISTS "${LOX_ECOSYSTEM_DIR}/safemath/safemath.h")
    target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/safemath")
    target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_SAFEMATH=1)
endif()

if(EXISTS "${LOX_ECOSYSTEM_DIR}/microlog/include/mlog.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microlog/src/mlog.c")
    target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microlog/src/mlog.c")
    target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microlog/include")
    target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROLOG=1)
endif()

if(EXISTS "${LOX_ECOSYSTEM_DIR}/microhealth/include/mhealth.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microhealth/src/mhealth.c")
    target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microhealth/src/mhealth.c")
    target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microhealth/include")
    target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROHEALTH=1)
endif()

if(LOXGUARD_USE_MICROTIMER)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microtimer/include/mtimer.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microtimer/src/mtimer.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microtimer/src/mtimer.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microtimer/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROTIMER=1 LOXGUARD_USE_MICROTIMER=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROTIMER=ON but microtimer companion library not fully available; building without microtimer backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROWDT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microwdt/include/mwdt.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microwdt/src/mwdt.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microwdt/src/mwdt.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microwdt/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROWDT=1 LOXGUARD_USE_MICROWDT=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROWDT=ON but microwdt companion library not fully available; building without microwdt backend.")
    endif()
endif()

if(LOXGUARD_USE_NVLOG)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/nvlog/include/nvlog.h" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/nvlog/src/nvlog.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/nvlog/backends/nvlog_posix.h" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/nvlog/backends/nvlog_posix.c")
        target_sources(loxguard PRIVATE
            "${LOX_ECOSYSTEM_DIR}/nvlog/src/nvlog.c"
            "${LOX_ECOSYSTEM_DIR}/nvlog/backends/nvlog_posix.c"
        )
        target_include_directories(loxguard PRIVATE
            "${LOX_ECOSYSTEM_DIR}/nvlog/include"
            "${LOX_ECOSYSTEM_DIR}/nvlog/backends"
        )
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_NVLOG=1 LOXGUARD_USE_NVLOG=1)
    else()
        message(WARNING "LOXGUARD_USE_NVLOG=ON but nvlog companion library not fully available; building without nvlog adapter backend.")
    endif()
endif()

add_executable(loxguard_test
    tests/test_common.c
    tests/test_span.c
    tests/test_arena.c
    tests/test_pipeline.c
    tests/test_rtos_bridge.c
    tests/test_guarded_module.c
    tests/test_ports.c
    tests/test_main.c
    examples/guarded/guarded_parser.c
)

target_include_directories(loxguard_test PRIVATE tests)
target_link_libraries(loxguard_test PRIVATE loxguard)

add_executable(loxguard_demo
    examples/host_demo.c
)

target_link_libraries(loxguard_demo PRIVATE loxguard)

add_executable(loxguard_backend_demo
    examples/host_backend_demo.c
)

target_link_libraries(loxguard_backend_demo PRIVATE loxguard)

enable_testing()
add_test(NAME loxguard_test COMMAND $<TARGET_FILE:loxguard_test>)
add_test(NAME loxguard_backend_demo COMMAND $<TARGET_FILE:loxguard_backend_demo>)
add_test(
    NAME loxguard_static_check
    COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/guard_static_check.ps1 -Path ${CMAKE_SOURCE_DIR}/src
)
add_test(
    NAME loxguard_static_check_guarded
    COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/guard_static_check.ps1 -Path ${CMAKE_SOURCE_DIR}/examples/guarded
)
