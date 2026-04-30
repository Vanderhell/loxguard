cmake_minimum_required(VERSION 3.16)
project(loxguard C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
option(LOXGUARD_USE_NVLOG "Enable nvlog host/RAM persistence adapter" OFF)
option(LOXGUARD_USE_MICROTIMER "Enable microtimer companion timing adapter path" OFF)
option(LOXGUARD_USE_MICROWDT "Enable microwdt companion watchdog/liveness adapter path" OFF)
option(LOXGUARD_USE_MICROASSERT "Enable microassert companion adapter path when available" OFF)
option(LOXGUARD_USE_PANICDUMP "Enable panicdump companion adapter path when available" OFF)
option(LOXGUARD_USE_MICRORES "Enable microres companion recovery/circuit-breaker adapter path" OFF)
option(LOXGUARD_USE_MICROBOOT "Enable microboot companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROBUS "Enable microbus companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROCBOR "Enable microcbor companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROFLASH "Enable microflash companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROSH "Enable microsh companion adapter path when available" OFF)
option(LOXGUARD_USE_MICRORING "Enable microring companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROCONF "Enable microconf companion adapter path when available" OFF)
option(LOXGUARD_USE_MICROTEST "Enable microtest companion smoke test path when available" OFF)
option(LOXGUARD_USE_LOXDB "Enable loxdb companion event-history adapter path when available" OFF)

add_library(loxguard STATIC
    src/loxguard.c
    src/loxguard_adapters.c
    src/adapters/loxguard_nvlog_adapter.c
    src/adapters/loxguard_loxdb_adapter.c
    src/loxguard_backends.c
    src/loxguard_format.c
    src/loxguard_rtos_bridge.c
    src/loxguard_ports.c
    src/loxguard_shell.c
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

if(LOXGUARD_USE_MICROASSERT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microassert/include/massert.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microassert/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROASSERT=1 LOXGUARD_USE_MICROASSERT=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROASSERT=ON but microassert companion library is not available in ecosystem/ (or third_party fallback).")
    endif()
endif()

if(LOXGUARD_USE_PANICDUMP)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/panicdump/include/panicdump.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/panicdump/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_PANICDUMP=1 LOXGUARD_USE_PANICDUMP=1)
    else()
        message(WARNING "LOXGUARD_USE_PANICDUMP=ON but panicdump companion library is not available in ecosystem/ (or third_party fallback).")
    endif()
endif()

if(LOXGUARD_USE_MICRORES)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microres/include/mres.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microres/src/mres.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microres/src/mres.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microres/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICRORES=1 LOXGUARD_USE_MICRORES=1)
    else()
        message(WARNING "LOXGUARD_USE_MICRORES=ON but microres companion library is not available; building without microres backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROBUS)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microbus/include/mbus.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microbus/src/mbus.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microbus/src/mbus.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microbus/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROBUS=1 LOXGUARD_USE_MICROBUS=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROBUS=ON but microbus companion library is not available; building without microbus backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROSH)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microsh/include/msh.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microsh/src/msh.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microsh/src/msh.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microsh/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROSH=1 LOXGUARD_USE_MICROSH=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROSH=ON but microsh companion library is not available; building without microsh backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROBOOT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microboot/include/mboot.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microboot/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROBOOT=1 LOXGUARD_USE_MICROBOOT=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROBOOT=ON but microboot companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROCBOR)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microcbor/include/mcbor.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microcbor/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROCBOR=1 LOXGUARD_USE_MICROCBOR=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROCBOR=ON but microcbor companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROFLASH)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microflash/include/mflash.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microflash/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROFLASH=1 LOXGUARD_USE_MICROFLASH=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROFLASH=ON but microflash companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICRORING)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/micoring/include/micoring.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/micoring/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICRORING=1 LOXGUARD_USE_MICRORING=1)
    else()
        message(WARNING "LOXGUARD_USE_MICRORING=ON but micoring companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROCONF)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microconf/include/mconf.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microconf/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROCONF=1 LOXGUARD_USE_MICROCONF=1)
    else()
        message(WARNING "LOXGUARD_USE_MICROCONF=ON but microconf companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_LOXDB)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/include/lox.h" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/loxdb.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_kv.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_ts.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_rel.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_wal.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_crc.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/port/ram/lox_port_ram.c" AND
       EXISTS "${LOX_ECOSYSTEM_DIR}/loxdb/port/ram/lox_port_ram.h")
        target_sources(loxguard PRIVATE
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/loxdb.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_kv.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_ts.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_rel.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_wal.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src/lox_crc.c"
            "${LOX_ECOSYSTEM_DIR}/loxdb/port/ram/lox_port_ram.c"
        )
        target_include_directories(loxguard PRIVATE
            "${LOX_ECOSYSTEM_DIR}/loxdb/include"
            "${LOX_ECOSYSTEM_DIR}/loxdb/port/ram"
            "${LOX_ECOSYSTEM_DIR}/loxdb/src"
        )
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_LOXDB=1 LOXGUARD_USE_LOXDB=1)
    else()
        message(WARNING "LOXGUARD_USE_LOXDB=ON but loxdb companion library is not fully available; building without loxdb backend.")
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

get_target_property(LOXGUARD_ALL_SOURCES loxguard SOURCES)
add_library(loxguard_testlib STATIC ${LOXGUARD_ALL_SOURCES})
target_include_directories(loxguard_testlib PUBLIC include)

get_target_property(LOXGUARD_ALL_INCLUDES loxguard INCLUDE_DIRECTORIES)
if(LOXGUARD_ALL_INCLUDES)
    target_include_directories(loxguard_testlib PRIVATE ${LOXGUARD_ALL_INCLUDES})
endif()

get_target_property(LOXGUARD_ALL_DEFS loxguard COMPILE_DEFINITIONS)
if(LOXGUARD_ALL_DEFS)
    target_compile_definitions(loxguard_testlib PRIVATE ${LOXGUARD_ALL_DEFS})
endif()
target_compile_definitions(loxguard_testlib PRIVATE LOXGUARD_TESTING=1)

add_executable(loxguard_test
    tests/test_common.c
    tests/test_span.c
    tests/test_arena.c
    tests/test_pipeline.c
    tests/test_rtos_bridge.c
    tests/test_guarded_module.c
    tests/test_ports.c
    tests/test_shell.c
    tests/test_main.c
    examples/guarded/guarded_parser.c
)

target_include_directories(loxguard_test PRIVATE tests)
target_compile_definitions(loxguard_test PRIVATE LOXGUARD_TESTING=1)
target_link_libraries(loxguard_test PRIVATE loxguard_testlib)

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

if(LOXGUARD_USE_MICROTEST)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microtest/include/mtest.h")
        add_executable(loxguard_microtest_smoke tests/test_microtest_smoke.c)
        target_include_directories(loxguard_microtest_smoke PRIVATE "${LOX_ECOSYSTEM_DIR}/microtest/include")
        add_test(NAME loxguard_microtest_smoke COMMAND $<TARGET_FILE:loxguard_microtest_smoke>)
    else()
        message(WARNING "LOXGUARD_USE_MICROTEST=ON but microtest companion library is not available.")
    endif()
endif()
add_test(
    NAME loxguard_static_check_guarded
    COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/guard_static_check.ps1 -Path ${CMAKE_SOURCE_DIR}/examples/guarded
)
