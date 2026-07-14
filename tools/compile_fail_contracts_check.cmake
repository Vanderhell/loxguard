cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED LOXGUARD_SOURCE_DIR)
    message(FATAL_ERROR "LOXGUARD_SOURCE_DIR is required.")
endif()
if(NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "WORK_DIR is required.")
endif()

function(_run_expect_fail)
    execute_process(
        COMMAND ${ARGV}
        RESULT_VARIABLE _rv
    )
    if(_rv EQUAL 0)
        message(FATAL_ERROR "Expected failure but command succeeded: ${ARGV}")
    endif()
endfunction()

function(_run_expect_success)
    execute_process(
        COMMAND ${ARGV}
        RESULT_VARIABLE _rv
    )
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "Command failed (${_rv}): ${ARGV}")
    endif()
endfunction()

file(TO_CMAKE_PATH "${LOXGUARD_SOURCE_DIR}" _loxguard_source_dir)
set(_root "${WORK_DIR}/compile_fail_contracts")
file(MAKE_DIRECTORY "${_root}")

function(_write_project dir name defs body)
    file(REMOVE_RECURSE "${dir}")
    file(MAKE_DIRECTORY "${dir}")
    file(WRITE "${dir}/main.c" "#include \"loxguard.h\"\nint main(void) { return 0; }\n")
    file(WRITE "${dir}/CMakeLists.txt" "cmake_minimum_required(VERSION 3.16)\nproject(${name} LANGUAGES C)\n${defs}\nadd_subdirectory(\"${_loxguard_source_dir}\" loxguard_build EXCLUDE_FROM_ALL)\nadd_executable(${name} main.c)\ntarget_link_libraries(${name} PRIVATE loxguard::loxguard)\n${body}\n")
endfunction()

set(_bad_profile_src "${_root}/bad_profile")
file(REMOVE_RECURSE "${_bad_profile_src}")
file(MAKE_DIRECTORY "${_bad_profile_src}")
file(WRITE "${_bad_profile_src}/main.c" "#define LOXGUARD_PROFILE 99\n#include \"loxguard_profiles.h\"\nint main(void) { return 0; }\n")
file(WRITE "${_bad_profile_src}/CMakeLists.txt" "cmake_minimum_required(VERSION 3.16)\nproject(loxguard_bad_profile LANGUAGES C)\nadd_executable(loxguard_bad_profile main.c)\ntarget_include_directories(loxguard_bad_profile PRIVATE \"${_loxguard_source_dir}/include\")\n")
_run_expect_success(${CMAKE_COMMAND} -S "${_bad_profile_src}" -B "${_bad_profile_src}/build" "-DCMAKE_BUILD_TYPE=Debug")
_run_expect_fail(${CMAKE_COMMAND} --build "${_bad_profile_src}/build" --config Debug)

set(_bad_events_src "${_root}/bad_events")
_write_project(
    "${_bad_events_src}"
    "loxguard_bad_events"
    "add_compile_definitions(LOX_BLACKBOX_MAX_EVENTS=8)"
    ""
)
_run_expect_success(${CMAKE_COMMAND} -S "${_bad_events_src}" -B "${_bad_events_src}/build" "-DCMAKE_BUILD_TYPE=Debug")
_run_expect_fail(${CMAKE_COMMAND} --build "${_bad_events_src}/build" --config Debug)

set(_bad_str_src "${_root}/bad_str")
_write_project(
    "${_bad_str_src}"
    "loxguard_bad_str"
    "add_compile_definitions(LOX_BLACKBOX_STR_MAX=32)"
    ""
)
_run_expect_success(${CMAKE_COMMAND} -S "${_bad_str_src}" -B "${_bad_str_src}/build" "-DCMAKE_BUILD_TYPE=Debug")
_run_expect_fail(${CMAKE_COMMAND} --build "${_bad_str_src}/build" --config Debug)

set(_bad_caps_src "${_root}/bad_caps")
file(REMOVE_RECURSE "${_bad_caps_src}")
file(MAKE_DIRECTORY "${_bad_caps_src}")
file(WRITE "${_bad_caps_src}/main.c" "#include \"loxguard.h\"\nint main(void) { return 0; }\n")
file(WRITE "${_bad_caps_src}/CMakeLists.txt" "cmake_minimum_required(VERSION 3.16)\nproject(loxguard_bad_caps LANGUAGES C)\nadd_subdirectory(\"${_loxguard_source_dir}\" loxguard_build EXCLUDE_FROM_ALL)\nadd_executable(loxguard_bad_caps main.c)\ntarget_link_libraries(loxguard_bad_caps PRIVATE loxguard::loxguard)\n")
_run_expect_fail(${CMAKE_COMMAND} -S "${_bad_caps_src}" -B "${_bad_caps_src}/build" "-DCMAKE_BUILD_TYPE=Debug" -DLOXGUARD_STRICT_COMPANION_GATES=ON -DLOXGUARD_USE_MICROASSERT=ON)
