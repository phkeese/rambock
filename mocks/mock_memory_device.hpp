#pragma once
#include "../memory_device.hpp"
#include <cstdlib>
#include <memory>

namespace rambock {

template <size_t S> struct MockMemoryDevice : public MemoryDevice {

	MockMemoryDevice()
		: MemoryDevice{}
		, _memory{} {}

	virtual void *read(Address from, void *to, Size count) override {
		return std::memcpy(to, to_address(from), count);
	}

	virtual Address write(Address to, const void *from, Size count) override {
		std::memcpy(to_address(to), from, count);
		return to;
	}

  private:
	inline uint8_t *to_address(Address address) {
		return &_memory[address];
	}
	uint8_t _memory[S];
};

} // namespace rambock