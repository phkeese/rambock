#pragma once
#include "base_layer.hpp"
#include <stdlib.h>

namespace rambock {
namespace layers {

template <size_t S> struct CacheLayer : public MemoryLayer {
	explicit CacheLayer(MemoryDevice &memory_device);

	virtual void *read(Address from, void *to, Size count) override;
	virtual Address write(Address to, const void *from, Size count) override;

  private:
	/**
	 * @brief Cache an address if possible
	 * @note Returns nullptr if caching is impossible
	 * @param address First address to cache
	 * @param count Number of bytes to cache
	 * @return Address in local memory
	 */
	void *cache(Address address, Size count);

	bool is_cached(Address address, Size count);
	void evict();
	void fetch(Address address, Size count);

	Address _begin, _end;
	uint8_t _cache[S];
};

template <size_t S>
CacheLayer<S>::CacheLayer(MemoryDevice &memory_device)
	: MemoryLayer(memory_device)
	, _begin{}
	, _end{}
	, _cache{} {}

template <size_t S>
void *CacheLayer<S>::read(Address from, void *to, Size count) {
	return nullptr;
}

template <size_t S>
Address CacheLayer<S>::write(Address to, const void *from, Size count) {
	return Address();
}

template <size_t S> void *CacheLayer<S>::cache(Address address, Size count) {
	if (count > S) {
		// Too large to cache
		return nullptr;
	} else if (is_cached()) {
		// Already in cache
		Size offset = address - _begin;
		return static_cast<void *>(&_cache[offset]);
	} else {
		// Cache new range
		evict();
		fetch(address, count);
		return static_cast<void *>(&_cache);
	}
}

template <size_t S> bool CacheLayer<S>::is_cached(Address address, Size count) {
	Address range_begin = address;
	Address range_end = address + count;
	return _begin <= range_begin && range_end <= _end;
}

template <size_t S> void CacheLayer<S>::evict() {
	memory_device().write(_begin, &_cache, _end - _begin);
}

template <size_t S> void CacheLayer<S>::fetch(Address address, Size count) {
	_begin = address;
	_end = address + count;
	memory_device().read(address, &_cache, count);
}

} // namespace layers
} // namespace rambock
