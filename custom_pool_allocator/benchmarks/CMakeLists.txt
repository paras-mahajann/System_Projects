include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(benchmarks bench_pool_allocator.cpp)
target_link_libraries(benchmarks PRIVATE pool_allocator benchmark::benchmark_main)