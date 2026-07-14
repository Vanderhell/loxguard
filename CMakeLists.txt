cmake_minimum_required(VERSION 3.16)
project(loxguard VERSION 1.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(GNUInstallDirs)

set(LOXGUARD_IS_TOP_LEVEL OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LOXGUARD_IS_TOP_LEVEL ON)
endif()

option(LOXGUARD_BUILD_TESTS "Build tests and demo executables (default ON only for top-level builds)" ${LOXGUARD_IS_TOP_LEVEL})
option(LOXGUARD_ENABLE_STRICT_WARNINGS "Enable strict warnings for project-owned sources" ON)
option(LOXGUARD_STRICT_COMPANION_GATES "Fail when requested companion sources are unavailable" OFF)
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

function(loxguard_report_missing_companion message)
    if(LOXGUARD_STRICT_COMPANION_GATES)
        message(FATAL_ERROR "${message}")
    else()
        message(WARNING "${message}")
    endif()
endfunction()

function(loxguard_apply_strict_warnings)
    if(NOT LOXGUARD_ENABLE_STRICT_WARNINGS)
        return()
    endif()

    foreach(_src IN LISTS ARGN)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
            set_property(SOURCE "${_src}" APPEND PROPERTY COMPILE_OPTIONS
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wall>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wextra>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wpedantic>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wconversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wsign-conversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wshadow>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wstrict-prototypes>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wmissing-prototypes>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wformat=2>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Wundef>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU>>:-Werror>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wall>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wextra>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wpedantic>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wconversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wsign-conversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wshadow>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wstrict-prototypes>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wmissing-prototypes>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wformat=2>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Wundef>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:Clang>>:-Werror>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:MSVC>>:/W4>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:MSVC>>:/WX>"
                "$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:MSVC>>:/permissive->"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wall>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wextra>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wpedantic>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wconversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wsign-conversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wshadow>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wformat=2>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wundef>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Werror>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wall>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wextra>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wpedantic>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wconversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wsign-conversion>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wshadow>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wformat=2>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wundef>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Werror>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/W4>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/WX>"
                "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/permissive->"
            )
        endif()
    endforeach()
endfunction()

set(LOXGUARD_PROJECT_SOURCES
    src/loxguard.c
    src/loxguard_adapters.c
    src/loxguard_backends.c
    src/loxguard_format.c
    src/loxguard_ports.c
    src/loxguard_rtos_bridge.c
    src/loxguard_shell.c
    src/adapters/loxguard_nvlog_adapter.c
    src/adapters/loxguard_loxdb_adapter.c
)

add_library(loxguard STATIC ${LOXGUARD_PROJECT_SOURCES})

add_library(loxguard::loxguard ALIAS loxguard)

target_include_directories(loxguard
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

loxguard_apply_strict_warnings(${LOXGUARD_PROJECT_SOURCES})

set(LOX_ECOSYSTEM_DIR "" CACHE PATH "Path to loxguard ecosystem/ or third_party/ root. When empty, loxguard auto-discovers.")
if(LOX_ECOSYSTEM_DIR STREQUAL "")
    set(_lox_ecosystem_candidates
        "${CMAKE_CURRENT_SOURCE_DIR}/ecosystem"
        "${CMAKE_SOURCE_DIR}/ecosystem"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
        "${CMAKE_SOURCE_DIR}/third_party"
    )
    foreach(_cand IN LISTS _lox_ecosystem_candidates)
        if(EXISTS "${_cand}")
            set(LOX_ECOSYSTEM_DIR "${_cand}" CACHE PATH "Path to loxguard ecosystem/ or third_party/ root. When empty, loxguard auto-discovers." FORCE)
            break()
        endif()
    endforeach()
    unset(_lox_ecosystem_candidates)
    unset(_cand)
endif()

set(LOXGUARD_ANY_REQUESTED_COMPANION OFF)
foreach(_loxguard_requested_companion IN ITEMS
    LOXGUARD_USE_NVLOG
    LOXGUARD_USE_MICROTIMER
    LOXGUARD_USE_MICROWDT
    LOXGUARD_USE_MICROASSERT
    LOXGUARD_USE_PANICDUMP
    LOXGUARD_USE_MICRORES
    LOXGUARD_USE_MICROBOOT
    LOXGUARD_USE_MICROBUS
    LOXGUARD_USE_MICROCBOR
    LOXGUARD_USE_MICROFLASH
    LOXGUARD_USE_MICROSH
    LOXGUARD_USE_MICRORING
    LOXGUARD_USE_MICROCONF
    LOXGUARD_USE_MICROTEST
    LOXGUARD_USE_LOXDB)
    if(${_loxguard_requested_companion})
        set(LOXGUARD_ANY_REQUESTED_COMPANION ON)
        break()
    endif()
endforeach()
unset(_loxguard_requested_companion)

if(LOX_ECOSYSTEM_DIR STREQUAL "" OR NOT EXISTS "${LOX_ECOSYSTEM_DIR}")
    if(LOXGUARD_ANY_REQUESTED_COMPANION)
        loxguard_report_missing_companion("One or more LOXGUARD_USE_* companion options were requested but the ecosystem root is not available; set LOX_ECOSYSTEM_DIR or disable the requested companion gates.")
    endif()
    message(STATUS "loxguard: ecosystem root not found; building core-only.")
endif()

if(LOX_ECOSYSTEM_DIR AND EXISTS "${LOX_ECOSYSTEM_DIR}")
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
        loxguard_report_missing_companion("LOXGUARD_USE_MICROTIMER=ON but microtimer companion library not fully available; building without microtimer backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROWDT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microwdt/include/mwdt.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microwdt/src/mwdt.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microwdt/src/mwdt.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microwdt/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROWDT=1 LOXGUARD_USE_MICROWDT=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROWDT=ON but microwdt companion library not fully available; building without microwdt backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROASSERT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microassert/include/massert.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microassert/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROASSERT=1 LOXGUARD_USE_MICROASSERT=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROASSERT=ON but microassert companion library is not available in ecosystem/ (or third_party fallback).")
    endif()
endif()

if(LOXGUARD_USE_PANICDUMP)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/panicdump/include/panicdump.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/panicdump/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_PANICDUMP=1 LOXGUARD_USE_PANICDUMP=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_PANICDUMP=ON but panicdump companion library is not available in ecosystem/ (or third_party fallback).")
    endif()
endif()

if(LOXGUARD_USE_MICRORES)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microres/include/mres.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microres/src/mres.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microres/src/mres.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microres/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICRORES=1 LOXGUARD_USE_MICRORES=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICRORES=ON but microres companion library is not available; building without microres backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROBUS)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microbus/include/mbus.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microbus/src/mbus.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microbus/src/mbus.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microbus/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROBUS=1 LOXGUARD_USE_MICROBUS=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROBUS=ON but microbus companion library is not available; building without microbus backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROSH)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microsh/include/msh.h" AND EXISTS "${LOX_ECOSYSTEM_DIR}/microsh/src/msh.c")
        target_sources(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microsh/src/msh.c")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microsh/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROSH=1 LOXGUARD_USE_MICROSH=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROSH=ON but microsh companion library is not available; building without microsh backend.")
    endif()
endif()

if(LOXGUARD_USE_MICROBOOT)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microboot/include/mboot.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microboot/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROBOOT=1 LOXGUARD_USE_MICROBOOT=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROBOOT=ON but microboot companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROCBOR)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microcbor/include/mcbor.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microcbor/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROCBOR=1 LOXGUARD_USE_MICROCBOR=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROCBOR=ON but microcbor companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROFLASH)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microflash/include/mflash.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microflash/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROFLASH=1 LOXGUARD_USE_MICROFLASH=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROFLASH=ON but microflash companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICRORING)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/micoring/include/micoring.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/micoring/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICRORING=1 LOXGUARD_USE_MICRORING=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICRORING=ON but micoring companion library is not available.")
    endif()
endif()

if(LOXGUARD_USE_MICROCONF)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microconf/include/mconf.h")
        target_include_directories(loxguard PRIVATE "${LOX_ECOSYSTEM_DIR}/microconf/include")
        target_compile_definitions(loxguard PRIVATE LOXGUARD_HAVE_MICROCONF=1 LOXGUARD_USE_MICROCONF=1)
    else()
        loxguard_report_missing_companion("LOXGUARD_USE_MICROCONF=ON but microconf companion library is not available.")
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
        loxguard_report_missing_companion("LOXGUARD_USE_LOXDB=ON but loxdb companion library is not fully available; building without loxdb backend.")
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
        loxguard_report_missing_companion("LOXGUARD_USE_NVLOG=ON but nvlog companion library not fully available; building without nvlog adapter backend.")
    endif()
endif()

endif() # LOX_ECOSYSTEM_DIR exists

install(
    TARGETS loxguard
    EXPORT loxguardTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/loxguardConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/loxguardConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/loxguard
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/loxguardConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    EXPORT loxguardTargets
    NAMESPACE loxguard::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/loxguard
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/loxguardConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/loxguardConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/loxguard
)

if(LOXGUARD_BUILD_TESTS)
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
    tests/test_phase01_core.c
    tests/test_span.c
    tests/test_arena.c
    tests/test_pipeline.c
    tests/test_pipeline_legacy.c
    tests/test_loxguard_run.c
    tests/test_rtos_bridge.c
    tests/test_guarded_module.c
    tests/test_ports.c
    tests/test_shell.c
    tests/test_main.c
    examples/guarded/guarded_parser.c
)

loxguard_apply_strict_warnings(
    tests/test_common.c
    tests/test_phase01_core.c
    tests/test_span.c
    tests/test_arena.c
    tests/test_pipeline.c
    tests/test_pipeline_legacy.c
    tests/test_loxguard_run.c
    tests/test_rtos_bridge.c
    tests/test_guarded_module.c
    tests/test_ports.c
    tests/test_shell.c
    tests/test_main.c
    examples/guarded/guarded_parser.c
    examples/host_demo.c
    examples/host_backend_demo.c
)

target_include_directories(loxguard_test PRIVATE tests examples/guarded)
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
find_program(POWERSHELL_EXE NAMES pwsh powershell)
if(POWERSHELL_EXE)
    add_test(
        NAME loxguard_static_check
        COMMAND ${POWERSHELL_EXE} -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/guard_static_check.ps1 -Path ${CMAKE_SOURCE_DIR}/src
    )
else()
    message(STATUS "PowerShell not found; skipping loxguard_static_check tests.")
endif()

if(LOXGUARD_USE_MICROTEST)
    if(EXISTS "${LOX_ECOSYSTEM_DIR}/microtest/include/mtest.h")
        add_executable(loxguard_microtest_smoke tests/test_microtest_smoke.c)
        target_include_directories(loxguard_microtest_smoke PRIVATE "${LOX_ECOSYSTEM_DIR}/microtest/include")
        add_test(NAME loxguard_microtest_smoke COMMAND $<TARGET_FILE:loxguard_microtest_smoke>)
    else()
        message(WARNING "LOXGUARD_USE_MICROTEST=ON but microtest companion library is not available.")
    endif()
endif()
if(POWERSHELL_EXE)
    add_test(
        NAME loxguard_static_check_guarded
        COMMAND ${POWERSHELL_EXE} -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/guard_static_check.ps1 -Path ${CMAKE_SOURCE_DIR}/examples/guarded
    )
endif()

add_test(
    NAME loxguard_blackbox_layout_compat
    COMMAND
        ${CMAKE_COMMAND}
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DWORK_DIR=${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/tools/blackbox_layout_compat_check.cmake
)

add_test(
    NAME loxguard_consumer_add_subdirectory
    COMMAND
        ${CMAKE_COMMAND}
        -DMODE=add_subdirectory
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DWORK_DIR=${CMAKE_BINARY_DIR}/consumer_add_subdirectory
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCONFIG=$<CONFIG>
        -P ${CMAKE_SOURCE_DIR}/tools/consumer_smoke_test.cmake
)
set_tests_properties(loxguard_consumer_add_subdirectory PROPERTIES LABELS "consumer")

add_test(
    NAME loxguard_consumer_install_find_package
    COMMAND
        ${CMAKE_COMMAND}
        -DMODE=install_find_package
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DWORK_DIR=${CMAKE_BINARY_DIR}/consumer_install_find_package
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCONFIG=$<CONFIG>
        -P ${CMAKE_SOURCE_DIR}/tools/consumer_smoke_test.cmake
)
set_tests_properties(loxguard_consumer_install_find_package PROPERTIES LABELS "consumer")

add_test(
    NAME loxguard_consumer_install_find_package_cpp
    COMMAND
        ${CMAKE_COMMAND}
        -DMODE=install_find_package_cpp
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DWORK_DIR=${CMAKE_BINARY_DIR}/consumer_install_find_package_cpp
        -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCONFIG=$<CONFIG>
        -P ${CMAKE_SOURCE_DIR}/tools/consumer_smoke_test.cmake
)
set_tests_properties(loxguard_consumer_install_find_package_cpp PROPERTIES LABELS "consumer")

add_test(
    NAME loxguard_compile_fail_contracts
    COMMAND
        ${CMAKE_COMMAND}
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DWORK_DIR=${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/tools/compile_fail_contracts_check.cmake
)

add_test(
    NAME loxguard_version_consistency
    COMMAND
        ${CMAKE_COMMAND}
        -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DTAG=v${PROJECT_VERSION}
        -P ${CMAKE_SOURCE_DIR}/tools/version_consistency_check.cmake
)

find_program(ARM_NONE_EABI_GCC NAMES arm-none-eabi-gcc)
if(ARM_NONE_EABI_GCC)
    add_test(
        NAME loxguard_arm_compile_only
        COMMAND
            ${CMAKE_COMMAND}
            -DLOXGUARD_SOURCE_DIR=${CMAKE_SOURCE_DIR}
            -DARM_NONE_EABI_GCC=${ARM_NONE_EABI_GCC}
            -DWORK_DIR=${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/tools/arm_cortexm_compile_only.cmake
    )
endif()
endif()
