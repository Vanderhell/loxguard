cmake_minimum_required(VERSION 3.16)
project(loxguard_consumer_install_find_package_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(loxguard CONFIG REQUIRED)

add_executable(consumer_install_find_package_cpp main.cpp)
target_link_libraries(consumer_install_find_package_cpp PRIVATE loxguard::loxguard)
target_compile_options(consumer_install_find_package_cpp PRIVATE
    "$<$<CXX_COMPILER_ID:GNU>:-Wall>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wpedantic>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wconversion>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wsign-conversion>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wshadow>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wformat=2>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wundef>"
    "$<$<CXX_COMPILER_ID:GNU>:-Werror>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wall>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wextra>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wpedantic>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wconversion>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wsign-conversion>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wshadow>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wformat=2>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wundef>"
    "$<$<CXX_COMPILER_ID:Clang>:-Werror>"
    "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
    "$<$<CXX_COMPILER_ID:MSVC>:/WX>"
    "$<$<CXX_COMPILER_ID:MSVC>:/permissive->"
)

enable_testing()
add_test(NAME consumer_install_find_package_cpp_runs COMMAND consumer_install_find_package_cpp)
