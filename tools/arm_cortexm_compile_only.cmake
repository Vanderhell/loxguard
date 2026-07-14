cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED LOXGUARD_SOURCE_DIR)
    message(FATAL_ERROR "LOXGUARD_SOURCE_DIR is required.")
endif()
if(NOT DEFINED ARM_NONE_EABI_GCC)
    message(FATAL_ERROR "ARM_NONE_EABI_GCC is required.")
endif()
if(NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "WORK_DIR is required.")
endif()

set(_build_dir "${WORK_DIR}/arm_cortexm_compile_only")
file(REMOVE_RECURSE "${_build_dir}")

execute_process(
    COMMAND
        ${CMAKE_COMMAND}
        -S "${LOXGUARD_SOURCE_DIR}"
        -B "${_build_dir}"
        -DLOXGUARD_BUILD_TESTS=OFF
        -DLOXGUARD_STRICT_COMPANION_GATES=ON
        -DLOXGUARD_ENABLE_STRICT_WARNINGS=ON
        -DCMAKE_C_COMPILER=${ARM_NONE_EABI_GCC}
        -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
    RESULT_VARIABLE _configure_rv
)
if(NOT _configure_rv EQUAL 0)
    message(FATAL_ERROR "ARM Cortex-M configure failed (${_configure_rv})")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build "${_build_dir}" --target loxguard
    RESULT_VARIABLE _build_rv
)
if(NOT _build_rv EQUAL 0)
    message(FATAL_ERROR "ARM Cortex-M compile-only build failed (${_build_rv})")
endif()
