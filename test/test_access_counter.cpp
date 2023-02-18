#include "../layers/access_counter.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace layers;
using namespace mocks;

TEST_CASE("test access counter", "[layers]") {
	constexpr Size memory_size = 1024;
	constexpr Size buffer_size = 100;
	uint8_t buffer[buffer_size];
	Address address = Address(10);

	MockMemoryDevice<memory_size> memory_device{};
	AccessCounter counter{memory_device};

	SECTION("initial counts are zero") {
		REQUIRE(counter.reads() == 0);
		REQUIRE(counter.writes() == 0);
	}

	SECTION("counts are kept separate") {
		counter.read(address, &buffer, buffer_size);

		REQUIRE(counter.reads() == 1);
		REQUIRE(counter.writes() == 0);

		counter.write(address, &buffer, buffer_size);

		REQUIRE(counter.reads() == 1);
		REQUIRE(counter.writes() == 1);
	}

	SECTION("reset resets to zero") {
		counter.read(address, &buffer, buffer_size);

		REQUIRE(counter.reads() == 1);

		counter.reset();

		REQUIRE(counter.reads() == 0);
	}
}