#include "../external_ptr.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <iostream>

rambock::external_ptr<int> generate(rambock::MemoryDevice &memoryDevice) {
	return rambock::external_ptr<int>{memoryDevice, 0};
}

int main() {
	rambock::MockMemoryDevice<64 * 1024> memory{};
	rambock::external_ptr<int> ptr{memory, 0};
	auto second = ptr;
	auto third = generate(memory);
	third = second;
	third = generate(memory);
	std::cout << &ptr.memoryDevice();
}
