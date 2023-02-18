#include "../allocators/simple_allocator.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace allocators;
using namespace mocks;

TEST_CASE("Simple allocator allocates memory", "[allocators]") {
	constexpr Size memory_size = 1024;
	MockMemoryDevice<memory_size> memory_device{};
	SimpleAllocator allocator{memory_device, Address(memory_size)};
	allocator.begin();

	SECTION("Allocation returns an address") {
		Address address = allocator.allocate(100);
		REQUIRE(address);
	}

	SECTION("Allocations do not overlap") {
		Address a = allocator.allocate(100);
		Address b = allocator.allocate(100);

		REQUIRE(a + 100 < b);
	}

	SECTION("Freed memory is reused") {
		Address a = allocator.allocate(100);
		Address b = allocator.allocate(100);
		allocator.free(b);
		Address c = allocator.allocate(100);
		REQUIRE(b == c);
	}

	SECTION("Allocations count against free bytes") {
		Size free_bytes = allocator.get_free_bytes();
		allocator.allocate(100);
		REQUIRE(allocator.get_free_bytes() < free_bytes);
	}

	SECTION("Frees count towards free bytes") {
		Address address = allocator.allocate(100);
		Size before = allocator.get_free_bytes();
		allocator.free(address);
		Size after = allocator.get_free_bytes();
		REQUIRE(before < after);
	}

	SECTION("Full free reclaims full memory") {
		Size before = allocator.get_free_bytes();
		Address address = allocator.allocate(100);
		allocator.free(address);
		Size after = allocator.get_free_bytes();

		REQUIRE(before == after);
	}

	SECTION("Too large allocations fail") {
		Size before = allocator.get_free_bytes();
		Address address = allocator.allocate(memory_size * 2);
		REQUIRE(!address);
		REQUIRE(allocator.get_free_bytes() == before);
	}
}
