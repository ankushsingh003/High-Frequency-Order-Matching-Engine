include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(OrderBookTest OrderBookTest.cpp)
target_link_libraries(OrderBookTest PRIVATE MatchingEngineCore GTest::gtest_main)

include(GoogleTest)
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/v1.8.3.zip
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(OrderBookBench Benchmark.cpp)
target_link_libraries(OrderBookBench PRIVATE MatchingEngineCore benchmark::benchmark benchmark::benchmark_main)
