#include "../layers/cache_layer.hpp"
#include "../mocks/mock_memory_device.hpp"
#include "../mocks/mock_slow_layer.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace mocks;
using namespace layers;

TEST_CASE("benchmark cached access", "[benchmarks]") {
	using Data = int;
	constexpr Size memory_size = 1024;
	constexpr Size cache_size = sizeof(Data) * 16;
	constexpr Size N = memory_size / sizeof(Data);
	MemoryDevice *memory_bus = nullptr;
	memory_bus = new MockMemoryDevice<memory_size>{};
	memory_bus = new MockSlowLayer<1>{*memory_bus};
	MemoryDevice& uncached = *memory_bus;
	memory_bus = new CacheLayer<cache_size>{*memory_bus};
	MemoryDevice& cached = *memory_bus;

	auto fill_data_single = [](MemoryDevice &memory_device, int bias) {
		for (Size i = 0; i < N; i++) {
			Address address = Address::null() + sizeof(Data) * i;
			Data data = static_cast<int>(i + bias);
			memory_device.write(address, &data, sizeof(data));
		}
	};

	auto validate_data = [](MemoryDevice &memory_device, int bias) {
		for (Size i = 0; i < N; i++) {
			Address address = Address::null() + sizeof(Data) * i;
			Data data{};
			memory_device.read(&data, address, sizeof(data));
			if (data != static_cast<int>(i + bias)) {
				return false;
			}
		}
		return true;
	};

	auto test = [&](MemoryDevice &memory_device, int bias) {
		Catch::Timer timer{};
		timer.start();
		fill_data_single(memory_device, bias);
		auto time = timer.getElapsedMicroseconds();
		REQUIRE(validate_data(memory_device, bias));
		return time;
	};

	SECTION("linear access is faster") {
		auto with_cache = test(cached, 1);
		auto without_cache = test(uncached, 2);

		REQUIRE(with_cache <= without_cache);
	}
}
