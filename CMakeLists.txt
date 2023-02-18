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
        allocators/template_allocator.hpp
        examples/simple_usage.cpp
        external_ptr.hpp
        layers/base_layer.hpp
        memory_device.hpp
        mocks/mock_memory_device.hpp
        rambock_common.hpp
        )

add_executable(example
        examples/simple_usage.cpp
        )

target_link_libraries(example rambock)
