cmake_minimum_required(VERSION 3.21)
project(rambock)

set(CMAKE_CXX_STANDARD 11)

option(REQUIRE_STRICT_CONSTRAINTS "Require strict checks using <type_traits>" ON)
if (REQUIRE_STRICT_CONSTRAINTS)
    ADD_COMPILE_DEFINITIONS(STRICT_CHECKS)
endif ()

add_library(rambock
        allocators/base_allocator.hpp
        allocators/bump_allocator.hpp
        allocators/simple_allocator.hpp
        examples/simple_usage.cpp
        external_ptr.hpp
        helpers/template_allocator.hpp
        layers/access_counter.cpp
        layers/access_counter.hpp
        layers/base_layer.cpp
        layers/base_layer.hpp
        layers/cache_layer.hpp
        memory_device.hpp
        mocks/mock_memory_device.hpp
        mocks/mock_slow_layer.hpp
        rambock_common.hpp
        )

add_executable(example
        examples/simple_usage.cpp
        )

target_link_libraries(example rambock)

include(CTest)

find_package(Catch2 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(tests
        test/test_access_counter.cpp
        test/test_cache_layer.cpp
        test/test_core.cpp
        test/test_external_ptr.cpp
        test/test_simple_allocator.cpp
        )

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain rambock)

enable_testing()
add_test(test-simple-allocator tests [simple_allocator])
add_test(test-core tests [core])
add_test(test-external-ptr tests [external_ptr])
add_test(test-layers tests [layers])

add_executable(benchmark
        benchmarks/benchmark_cached_access.cpp
        )

target_link_libraries(benchmark PRIVATE Catch2::Catch2WithMain rambock)
add_test(benchmark-cached-access benchmark "benchmark cached access")
