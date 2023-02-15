cmake_minimum_required(VERSION 3.21)
project(rambock)

set(CMAKE_CXX_STANDARD 11)

add_library(rambock
        allocators/base_allocator.hpp
        allocators/bump_allocator.hpp
        allocators/simple_allocator.hpp
        examples/simple_usage.cpp
        external_ptr.hpp
        memory_device.hpp
        memory_layer.hpp
        mocks/mock_memory_device.hpp
        rambock_common.hpp
        )

add_executable(example
        examples/simple_usage.cpp
        )

target_link_libraries(example rambock)
