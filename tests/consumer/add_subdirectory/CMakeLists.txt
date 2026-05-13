cmake_minimum_required(VERSION 3.16)
project(loxguard_consumer_add_subdirectory LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(LOXGUARD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../.." loxguard_src)

add_executable(consumer_add_subdirectory main.c)
target_link_libraries(consumer_add_subdirectory PRIVATE loxguard::loxguard)

enable_testing()
add_test(NAME consumer_add_subdirectory_runs COMMAND consumer_add_subdirectory)
