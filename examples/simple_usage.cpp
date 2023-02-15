#include "../external_ptr.hpp"
#include "../memory_device.hpp"
#include <iostream>

rambock::external_ptr<int> generate(rambock::MemoryDevice*memoryDevice) {
	return rambock::external_ptr<int>{*memoryDevice, 0};
}

int main() {
	rambock::MemoryDevice *memoryDevice = nullptr;
	rambock::external_ptr<int> ptr{*memoryDevice, 0};
	auto second = ptr;
	auto third = generate(memoryDevice);
	third = second;
	third = generate(memoryDevice);
	std::cout << &ptr.memoryDevice();
}
