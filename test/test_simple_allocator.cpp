#include "../allocators/simple_allocator.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace allocators;

TEST_CASE("Simple allocator allocates memory", "[allocators]") {
	MockMemoryDevice<1024> memory_device{};
	SimpleAllocator allocator{memory_device, Address(1024)};

	SECTION("Allocation returns an address") {
		Address address = allocator.allocate(100);
		REQUIRE(address);
	}
}