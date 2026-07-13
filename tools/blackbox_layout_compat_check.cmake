cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED LOXGUARD_SOURCE_DIR)
    message(FATAL_ERROR "LOXGUARD_SOURCE_DIR is required.")
endif()
if(NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "WORK_DIR is required.")
endif()

function(_run)
    execute_process(
        COMMAND ${ARGV}
        RESULT_VARIABLE _rv
    )
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "Command failed (${_rv}): ${ARGV}")
    endif()
endfunction()

set(_root "${WORK_DIR}/blackbox_layout_compat")
set(_ok_src "${_root}/ok/src")
set(_ok_bld "${_root}/ok/build")
set(_bad_src "${_root}/bad/src")
set(_bad_events_bld "${_root}/bad/build_events")
set(_bad_str_bld "${_root}/bad/build_str")

file(MAKE_DIRECTORY "${_ok_src}" "${_bad_src}")

file(TO_CMAKE_PATH "${LOXGUARD_SOURCE_DIR}" _loxguard_source_dir)

set(_ok_cmake "cmake_minimum_required(VERSION 3.16)\n")
string(APPEND _ok_cmake "project(loxguard_blackbox_layout_ok LANGUAGES C CXX)\n")
string(APPEND _ok_cmake "add_subdirectory(\"${_loxguard_source_dir}\" loxguard_build EXCLUDE_FROM_ALL)\n")
string(APPEND _ok_cmake "add_executable(blackbox_layout_ok main.c c_part.c cpp_part.cpp)\n")
string(APPEND _ok_cmake "target_link_libraries(blackbox_layout_ok PRIVATE loxguard::loxguard)\n")
file(WRITE "${_ok_src}/CMakeLists.txt" "${_ok_cmake}")

file(WRITE "${_ok_src}/main.c" "#include \"loxguard.h\"\n\nint layout_check_c(void);\nint layout_check_cpp(void);\n\ntypedef char loxguard_blackbox_events_check[(LOX_BLACKBOX_MAX_EVENTS == 16u) ? 1 : -1];\ntypedef char loxguard_blackbox_str_check[(LOX_BLACKBOX_STR_MAX == 64u) ? 1 : -1];\ntypedef char loxguard_blackbox_storage_check[(sizeof(lox_blackbox_t) >= (16u * sizeof(lox_event_t))) ? 1 : -1];\n\nint main(void) {\n    return (layout_check_c() == 0 && layout_check_cpp() == 0) ? 0 : 1;\n}\n")

file(WRITE "${_ok_src}/c_part.c" "#include \"loxguard.h\"\n\nint layout_check_c(void) {\n    lox_blackbox_t bb;\n    (void)bb;\n    return (LOX_BLACKBOX_MAX_EVENTS == 16u && LOX_BLACKBOX_STR_MAX == 64u) ? 0 : 1;\n}\n")

file(WRITE "${_ok_src}/cpp_part.cpp" "#include \"loxguard.h\"\n\nstatic_assert(LOX_BLACKBOX_MAX_EVENTS == 16u, \"stable blackbox event count\");\nstatic_assert(LOX_BLACKBOX_STR_MAX == 64u, \"stable blackbox string size\");\n\nextern \"C\" int layout_check_cpp(void) {\n    lox_blackbox_t bb;\n    (void)bb;\n    return (LOX_BLACKBOX_MAX_EVENTS == 16u && LOX_BLACKBOX_STR_MAX == 64u) ? 0 : 1;\n}\n")

file(REMOVE_RECURSE "${_ok_bld}")
_run(${CMAKE_COMMAND} -S "${_ok_src}" -B "${_ok_bld}" "-DCMAKE_BUILD_TYPE=Debug")
_run(${CMAKE_COMMAND} --build "${_ok_bld}" --config Debug)

file(WRITE "${_bad_src}/main.c" "#include \"loxguard.h\"\nint main(void) { return 0; }\n")

file(REMOVE_RECURSE "${_bad_events_bld}")
set(_bad_events_cmake "cmake_minimum_required(VERSION 3.16)\n")
string(APPEND _bad_events_cmake "project(loxguard_blackbox_layout_bad_events LANGUAGES C)\n")
string(APPEND _bad_events_cmake "add_compile_definitions(LOX_BLACKBOX_MAX_EVENTS=8)\n")
string(APPEND _bad_events_cmake "add_subdirectory(\"${_loxguard_source_dir}\" loxguard_build EXCLUDE_FROM_ALL)\n")
string(APPEND _bad_events_cmake "add_executable(blackbox_layout_bad main.c)\n")
string(APPEND _bad_events_cmake "target_link_libraries(blackbox_layout_bad PRIVATE loxguard::loxguard)\n")
file(WRITE "${_bad_src}/CMakeLists.txt" "${_bad_events_cmake}")
execute_process(
    COMMAND ${CMAKE_COMMAND} -S "${_bad_src}" -B "${_bad_events_bld}" "-DCMAKE_BUILD_TYPE=Debug"
    RESULT_VARIABLE _bad_events_rv
)
if(_bad_events_rv EQUAL 0)
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build "${_bad_events_bld}" --config Debug
        RESULT_VARIABLE _bad_events_build_rv
    )
    if(_bad_events_build_rv EQUAL 0)
        message(FATAL_ERROR "Expected LOX_BLACKBOX_MAX_EVENTS override to fail.")
    endif()
endif()

file(REMOVE_RECURSE "${_bad_str_bld}")
set(_bad_str_cmake "cmake_minimum_required(VERSION 3.16)\n")
string(APPEND _bad_str_cmake "project(loxguard_blackbox_layout_bad_str LANGUAGES C)\n")
string(APPEND _bad_str_cmake "add_compile_definitions(LOX_BLACKBOX_STR_MAX=32)\n")
string(APPEND _bad_str_cmake "add_subdirectory(\"${_loxguard_source_dir}\" loxguard_build EXCLUDE_FROM_ALL)\n")
string(APPEND _bad_str_cmake "add_executable(blackbox_layout_bad main.c)\n")
string(APPEND _bad_str_cmake "target_link_libraries(blackbox_layout_bad PRIVATE loxguard::loxguard)\n")
file(WRITE "${_bad_src}/CMakeLists.txt" "${_bad_str_cmake}")
execute_process(
    COMMAND ${CMAKE_COMMAND} -S "${_bad_src}" -B "${_bad_str_bld}" "-DCMAKE_BUILD_TYPE=Debug"
    RESULT_VARIABLE _bad_str_rv
)
if(_bad_str_rv EQUAL 0)
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build "${_bad_str_bld}" --config Debug
        RESULT_VARIABLE _bad_str_build_rv
    )
    if(_bad_str_build_rv EQUAL 0)
        message(FATAL_ERROR "Expected LOX_BLACKBOX_STR_MAX override to fail.")
    endif()
endif()
