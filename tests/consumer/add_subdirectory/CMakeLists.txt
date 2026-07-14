cmake_minimum_required(VERSION 3.16)
project(loxguard_consumer_add_subdirectory LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(LOXGUARD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../.." loxguard_src)

add_executable(consumer_add_subdirectory main.c)
target_link_libraries(consumer_add_subdirectory PRIVATE loxguard::loxguard)
target_compile_options(consumer_add_subdirectory PRIVATE
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
add_test(NAME consumer_add_subdirectory_runs COMMAND consumer_add_subdirectory)
