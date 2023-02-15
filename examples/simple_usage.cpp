#include "../allocators/bump_allocator.hpp"
#include "../allocators/simple_allocator.hpp"
#include "../external_ptr.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <iostream>

rambock::external_ptr<int> generate(rambock::MemoryDevice &memoryDevice) {
	return rambock::external_ptr<int>{memoryDevice, 0};
}

int main() {
	constexpr size_t SIZE = 64 * 1024;
	rambock::MockMemoryDevice<SIZE> memory{};
	rambock::allocators::BumpAllocator allocator{memory, SIZE};
	rambock::allocators::SimpleAllocator simpleAllocator{memory, SIZE};

	rambock::external_ptr<int> ptr{memory, 0};
	auto second = ptr;
	auto third = generate(memory);
	third = second;
	third = generate(memory);
	std::cout << &ptr.memoryDevice();
}
