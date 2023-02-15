cmake_minimum_required(VERSION 3.21)
project(rambock)

set(CMAKE_CXX_STANDARD 11)

add_library(rambock
        allocators/bump_allocator.hpp
        allocators/simple_allocator.hpp
        external_ptr.hpp
        memory_allocator.hpp
        memory_device.hpp
        memory_layer.hpp
        rambock_common.hpp
        examples/simple_usage.cpp
        )

add_executable(example
        examples/simple_usage.cpp
        )