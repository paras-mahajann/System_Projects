cmake_minimum_required(VERSION 3.16)
project(pool_allocator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(pool_allocator INTERFACE)
target_include_directories(pool_allocator INTERFACE include)

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)