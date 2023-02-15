#include "../allocators/bump_allocator.hpp"
#include "../allocators/simple_allocator.hpp"
#include "../external_ptr.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <iostream>

using rambock::Address;
using rambock::allocators::BumpAllocator;
using rambock::allocators::SimpleAllocator;
using rambock::MockMemoryDevice;
using rambock::external_ptr;

rambock::external_ptr<int> generate(rambock::MemoryDevice &memoryDevice) {
	return rambock::external_ptr<int>{memoryDevice};
}

int main() {
	constexpr size_t SIZE = 64 * 1024;
	constexpr Address END = Address(SIZE);
	MockMemoryDevice<SIZE> memory{};
	BumpAllocator allocator{memory, END};
	SimpleAllocator simpleAllocator{memory, END};

	external_ptr<int> ptr{memory, rambock::null};
	auto second = ptr;
	auto third = generate(memory);
	third = second;
	third = generate(memory);
	std::cout << &ptr.memoryDevice();
}
