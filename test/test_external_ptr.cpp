#include "../allocators/bump_allocator.hpp"
#include "../external_ptr.hpp"
#include "../helpers/template_allocator.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace allocators;
using namespace mocks;
using namespace helpers;

TEST_CASE("Test external pointer semantics", "[external_ptr]") {
	constexpr Size memory_size = 1024;
	MockMemoryDevice<memory_size> memory_device{};
	BumpAllocator bump_allocator{memory_device, Address{memory_size}};
	TemplateAllocator allocator{bump_allocator};

	struct Data {
		int a;
		float b;

		bool operator==(const Data &rhs) const {
			return a == rhs.a && b == rhs.b;
		}
		bool operator!=(const Data &rhs) const { return !(rhs == *this); }
	};

	SECTION("Dereference operator allows access") {
		auto ptr = allocator.make_external<Data>();
		Data data{10, 3.1415};
		*ptr = data;
		Data read = *ptr;

		REQUIRE(read == data);

		ptr.free();
	}

	SECTION("Subscript operator allows access") {
		auto ptr = allocator.make_array<Data>(2);
		Data a{10, 3.1415};
		Data b{20, 1.5};

		ptr[0] = a;
		ptr[1] = b;

		Data read_a = ptr[0];
		Data read_b = ptr[1];

		REQUIRE(read_a == a);
		REQUIRE(read_b == b);

		ptr.free();
	}

	SECTION("Arrow operator accesses members") {
		auto ptr = allocator.make_external<Data>();
		Data data{10, 3.1415};
		*ptr = data;

		REQUIRE(data.a == ptr->a);
		REQUIRE(data.b == ptr->b);

		ptr.free();
	}

	SECTION("Offsets index into arrays") {
		auto ptr = allocator.make_array<Data>(2);
		auto a_ptr = ptr + 0;
		auto b_ptr = ptr + 1;

		REQUIRE(++a_ptr == b_ptr);
		REQUIRE(--a_ptr == ptr);

		ptr.free();
	}
}
