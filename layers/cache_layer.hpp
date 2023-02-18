#pragma once
#include "base_layer.hpp"
#include <memory.h>
#include <stdlib.h>

namespace rambock {
namespace layers {

template <size_t CacheSize> struct CacheLayer : public MemoryLayer {
	explicit CacheLayer(MemoryDevice &memory_device);

	virtual void *read(void *to, Address from, Size count) override;
	virtual Address write(Address to, const void *from, Size count) override;

	bool is_cached(Address address, Size count);
	void flush();
	void refresh();
	inline bool dirty() const { return _dirty; }

  private:
	/**
	 * @brief Cache an address if possible
	 * @note Returns nullptr if caching is impossible
	 * @param address First address to cache
	 * @param count Number of bytes to cache
	 * @return Address in local memory
	 */
	void *cache(Address address, Size count);

	void evict();
	void fetch(Address address);

	Address _begin, _end;
	uint8_t _cache[CacheSize];
	bool _dirty;
};

template <size_t S>
CacheLayer<S>::CacheLayer(MemoryDevice &memory_device)
	: MemoryLayer(memory_device)
	, _begin{}
	, _end{}
	, _cache{} {}

template <size_t S>
void *CacheLayer<S>::read(void *to, Address from, Size count) {
	void *cached_address = cache(from, count);
	if (cached_address) {
		return memcpy(to, cached_address, count);
	} else {
		// Flush first to ensure read consistency
		flush();
		return memory_device().read(to, from, count);
	}
}

template <size_t S>
Address CacheLayer<S>::write(Address to, const void *from, Size count) {
	void *cached_address = cache(to, count);
	if (cached_address) {
		_dirty = true;
		memcpy(cached_address, from, count);
		return to;
	} else {
		// Evict first to merge writes
		// evict, not flush because written need to be held consistent with
		// cache
		evict();
		return memory_device().write(to, from, count);
	}
}

template <size_t S> void *CacheLayer<S>::cache(Address address, Size count) {
	if (count > S) {
		// Too large to cache
		return nullptr;
	} else if (is_cached(address, count)) {
		// Already in cache
		Size offset = address - _begin;
		return static_cast<void *>(&_cache[offset]);
	} else {
		// Cache new range
		evict();
		fetch(address);
		return static_cast<void *>(&_cache);
	}
}

template <size_t S> bool CacheLayer<S>::is_cached(Address address, Size count) {
	Address range_begin = address;
	Address range_end = address + count;
	return _begin <= range_begin && range_end <= _end;
}

template <size_t S> void CacheLayer<S>::evict() {
	flush();
	_begin = _end = Address::null();
}

template <size_t S> void CacheLayer<S>::fetch(Address address) {
	_begin = address;
	_end = address + S;
	refresh();
}

template <size_t S> void CacheLayer<S>::flush() {
	if (!_dirty)
		return;
	memory_device().write(_begin, &_cache, _end - _begin);
	_dirty = false;
}

template <size_t S> void CacheLayer<S>::refresh() {
	memory_device().read(&_cache, _begin, S);
	_dirty = false;
}

} // namespace layers
} // namespace rambock
