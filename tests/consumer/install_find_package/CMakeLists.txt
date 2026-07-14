cmake_minimum_required(VERSION 3.16)
project(loxguard_consumer_install_find_package LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(loxguard CONFIG REQUIRED)

add_executable(consumer_install_find_package main.c)
target_link_libraries(consumer_install_find_package PRIVATE loxguard::loxguard)
target_compile_options(consumer_install_find_package PRIVATE
    "$<$<C_COMPILER_ID:GNU>:-Wall>"
    "$<$<C_COMPILER_ID:GNU>:-Wextra>"
    "$<$<C_COMPILER_ID:GNU>:-Wpedantic>"
    "$<$<C_COMPILER_ID:GNU>:-Wconversion>"
    "$<$<C_COMPILER_ID:GNU>:-Wsign-conversion>"
    "$<$<C_COMPILER_ID:GNU>:-Wshadow>"
    "$<$<C_COMPILER_ID:GNU>:-Wstrict-prototypes>"
    "$<$<C_COMPILER_ID:GNU>:-Wmissing-prototypes>"
    "$<$<C_COMPILER_ID:GNU>:-Wformat=2>"
    "$<$<C_COMPILER_ID:GNU>:-Wundef>"
    "$<$<C_COMPILER_ID:GNU>:-Werror>"
    "$<$<C_COMPILER_ID:Clang>:-Wall>"
    "$<$<C_COMPILER_ID:Clang>:-Wextra>"
    "$<$<C_COMPILER_ID:Clang>:-Wpedantic>"
    "$<$<C_COMPILER_ID:Clang>:-Wconversion>"
    "$<$<C_COMPILER_ID:Clang>:-Wsign-conversion>"
    "$<$<C_COMPILER_ID:Clang>:-Wshadow>"
    "$<$<C_COMPILER_ID:Clang>:-Wstrict-prototypes>"
    "$<$<C_COMPILER_ID:Clang>:-Wmissing-prototypes>"
    "$<$<C_COMPILER_ID:Clang>:-Wformat=2>"
    "$<$<C_COMPILER_ID:Clang>:-Wundef>"
    "$<$<C_COMPILER_ID:Clang>:-Werror>"
    "$<$<C_COMPILER_ID:MSVC>:/W4>"
    "$<$<C_COMPILER_ID:MSVC>:/WX>"
    "$<$<C_COMPILER_ID:MSVC>:/permissive->"
)

enable_testing()
add_test(NAME consumer_install_find_package_runs COMMAND consumer_install_find_package)
