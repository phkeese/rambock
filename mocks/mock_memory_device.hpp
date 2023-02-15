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
	inline void *to_address(Address address) {
		return static_cast<void *>(&_memory[0] + address);
	}
	uint8_t _memory[S];
};

} // namespace rambock