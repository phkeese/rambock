#include "../allocators/bump_allocator.hpp"
#include "../allocators/simple_allocator.hpp"
#include "../allocators/template_allocator.hpp"
#include "../external_ptr.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <iostream>

using rambock::Address;
using rambock::external_ptr;
using rambock::MockMemoryDevice;
using rambock::allocators::BumpAllocator;
using rambock::allocators::SimpleAllocator;
using rambock::allocators::TemplateAllocator;

rambock::external_ptr<int>
generate(rambock::allocators::BaseAllocator &allocator) {
	return rambock::external_ptr<int>{allocator};
}

int main() {
	constexpr size_t SIZE = 64 * 1024;
	constexpr Address END = Address(SIZE);
	MockMemoryDevice<SIZE> memory{};
	BumpAllocator bumpAllocator{memory, END};
	SimpleAllocator simpleAllocator{memory, END};

	TemplateAllocator allocator{bumpAllocator};

	auto ptr = allocator.make_external<int>(0);
	auto second = ptr;
	auto third = generate(simpleAllocator);
	third = second;
	third = generate(simpleAllocator);
	std::cout << &ptr.allocator();
}
