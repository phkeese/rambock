#pragma once

#include "memory_device.hpp"

namespace rambock {

template <typename T> struct external_ptr {

	constexpr external_ptr(MemoryDevice &memoryDevice)
		: external_ptr(memoryDevice, rambock::null) {}
	constexpr external_ptr(MemoryDevice &memoryDevice, const Address address)
		: _memoryDevice{&memoryDevice}
		, _address{address} {}
	constexpr external_ptr(const external_ptr &) = default;
	constexpr external_ptr(external_ptr &&) noexcept = default;
	external_ptr &operator=(const external_ptr &) = default;
	external_ptr &operator=(external_ptr &&) noexcept = default;

	inline MemoryDevice &memoryDevice() const { return *_memoryDevice; }
	inline Address address() const { return _address; }

  private:
	// @note Use a pointer to allow copy-assignment but enforce reference in
	// constructor
	MemoryDevice *_memoryDevice;
	Address _address;
};

} // namespace rambock