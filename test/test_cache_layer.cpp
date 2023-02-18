#include "../layers/access_counter.hpp"
#include "../layers/cache_layer.hpp"
#include "../mocks/mock_memory_device.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;
using namespace mocks;
using namespace layers;

TEST_CASE("cache layer caches accesses", "[layers]") {
	constexpr Size memory_size = 1024;
	constexpr Size cache_size = 64;
	struct Data {
		uint8_t bytes[10];
	} data;
	Address low_address = Address{10};
	Address low_cached_address = low_address + sizeof(Data);
	Address high_address = low_address + 2 * cache_size;

	MockMemoryDevice<memory_size> mock_memory_device{};
	AccessCounter access_counter{mock_memory_device};
	CacheLayer<cache_size> cache_layer{access_counter};

	SECTION("initial cache is empty") {
		REQUIRE(access_counter.reads() == 0);
		REQUIRE(access_counter.writes() == 0);
	}

	SECTION("uncached addresses force fetch") {
		REQUIRE(!cache_layer.is_cached(low_address, sizeof(Data)));

		int reads_before_fetch = access_counter.reads();
		cache_layer.read(low_address, &data, sizeof(data));

		REQUIRE(cache_layer.is_cached(low_address, sizeof(Data)));
		REQUIRE(access_counter.reads() > reads_before_fetch);
	}

	SECTION("cache eviction writes out cache contents") {
		int low_data = 10;
		cache_layer.write(low_address, &low_data, sizeof(low_data));

		int high_data = 20;
		cache_layer.write(high_address, &high_data, sizeof(high_data));

		int low_readback = 0;
		mock_memory_device.read(
			low_address, &low_readback, sizeof(low_readback));

		REQUIRE(low_data == low_readback);
	}

	SECTION("cached addresses hit cache") {
		cache_layer.read(low_address, &data, sizeof(data));
		int reads_before_cached_read = access_counter.reads();
		cache_layer.read(low_cached_address, &data, sizeof(data));
		REQUIRE(access_counter.reads() == reads_before_cached_read);
	}

	SECTION("cache serves if possible") {
		/*
		 * We construct the following situation:
		 * The memory behind the cache contains old information, the cache
		 * itself has updated data. When accessing a region larger than the
		 * cache, but overlapping the cached region, that data must equal that
		 * already in cache.
		 */
		struct Values {
			int i;
			float f;

			bool operator==(const Values &other) const {
				return i == other.i && f == other.f;
			};
		};

		Values values{10, 3.1415};
		// Write to memory
		mock_memory_device.write(low_address, &values, sizeof(values));

		// Write new data to cache
		Values new_values{20, 1.5};
		cache_layer.write(low_address, &new_values, sizeof(new_values));

		// Write back old values
		mock_memory_device.write(low_address, &values, sizeof(values));

		// Read more than cache size
		struct Large {
			Values values;
			uint8_t large_block[cache_size];
		} large;

		cache_layer.read(low_address, &large, sizeof(large));

		// The cache must be consulted first, even for accesses larger than the
		// cache size
		REQUIRE(large.values == new_values);

		// Write more than cache size and read back from cache
		large.values = values;
		cache_layer.write(low_address, &large, sizeof(large));
		cache_layer.read(low_address, &new_values, sizeof(new_values));
		REQUIRE(large.values == new_values);
	}

	SECTION("dirty flag marks changed content") {
		REQUIRE(!cache_layer.dirty());
		cache_layer.write(low_address, &data, sizeof(data));
		REQUIRE(cache_layer.dirty());
		cache_layer.flush();
		REQUIRE(!cache_layer.dirty());
		cache_layer.read(low_address, &data, sizeof(data));
		REQUIRE(!cache_layer.dirty());
		cache_layer.write(low_address, &data, sizeof(data));
		REQUIRE(cache_layer.dirty());
		cache_layer.refresh();
		REQUIRE(!cache_layer.dirty());
	}
}
