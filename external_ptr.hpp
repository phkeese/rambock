#pragma once

#include "memory_device.hpp"
#include "allocators/base_allocator.hpp"

namespace rambock {

template <typename T> struct external_ptr {
	using Allocator = rambock::allocators::BaseAllocator;

	constexpr external_ptr(Allocator &allocator)
		: external_ptr(allocator, Address::null()) {}
	constexpr external_ptr(Allocator &allocator, const Address address)
		: _allocator{&allocator}
		, _address{address} {}
	constexpr external_ptr(const external_ptr &) = default;
	constexpr external_ptr(external_ptr &&) noexcept = default;
	external_ptr &operator=(const external_ptr &) = default;
	external_ptr &operator=(external_ptr &&) noexcept = default;

	inline Allocator &allocator() const { return *_allocator; }
	inline Address address() const { return _address; }

  private:
	// @note Use a pointer to allow copy-assignment but enforce reference in
	// constructor
	Allocator *_allocator;
	Address _address;
};

} // namespace rambock