cmake_minimum_required(VERSION 3.16)
project(loxguard_consumer_install_find_package LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(loxguard CONFIG REQUIRED)

add_executable(consumer_install_find_package main.c)
target_link_libraries(consumer_install_find_package PRIVATE loxguard::loxguard)

enable_testing()
add_test(NAME consumer_install_find_package_runs COMMAND consumer_install_find_package)
