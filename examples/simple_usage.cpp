#include "../allocators/bump_allocator.hpp"
#include "../allocators/simple_allocator.hpp"
#include "../helpers/template_allocator.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <iostream>

using rambock::Address;
using rambock::external_ptr;
using rambock::allocators::BumpAllocator;
using rambock::allocators::SimpleAllocator;
using rambock::helpers::TemplateAllocator;
using rambock::mocks::MockMemoryDevice;

rambock::external_ptr<int>
generate(rambock::allocators::BaseAllocator &allocator) {
	return rambock::external_ptr<int>{allocator};
}

int main() {
	constexpr size_t memory_size = 1024;
	constexpr Address memory_end = Address(memory_size);
	MockMemoryDevice<memory_size> memory{};
	BumpAllocator bump_allocator{memory, memory_end};
	SimpleAllocator simple_allocator{memory, memory_end};

	TemplateAllocator allocator{bump_allocator};

	auto ptr = allocator.make_external<int>(0);
	auto second = ptr;
	auto third = generate(simple_allocator);
	third = second;
	third = generate(simple_allocator);
	*ptr = 10203040;
	std::cout << &ptr.allocator() << " " << *ptr << "\n";

	auto array = allocator.make_array<int>(10);
	for (int i = 0; i < 10; i++) {
		array[i] = i + 1;
		std::cout << array[i] << "\n";
	}

	std::cout << bump_allocator.get_free_bytes() << " bytes available\n";
}
