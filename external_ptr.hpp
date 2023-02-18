#pragma once

#include "allocators/base_allocator.hpp"
#include "local_copy.hpp"
#include "memory_device.hpp"

namespace rambock {

template <typename T> struct external_ptr {
	using Allocator = rambock::allocators::BaseAllocator;
	static constexpr Size allocation_size =
		sizeof(typename LocalCopy<T>::ExternalFrame);

	constexpr explicit external_ptr(Allocator &allocator)
		: _allocator{&allocator}
		, _address(Address::null()) {}
	constexpr external_ptr(Allocator &allocator, const Address address)
		: _allocator{&allocator}
		, _address{address} {}
	constexpr external_ptr(const external_ptr &) = default;
	constexpr external_ptr(external_ptr &&) noexcept = default;
	external_ptr &operator=(const external_ptr &) = default;
	external_ptr &operator=(external_ptr &&) noexcept = default;

	inline Allocator &allocator() const { return *_allocator; }
	inline Address address() const { return _address; }

	inline LocalCopy<T> operator->() { return this->operator*(); }
	inline LocalCopy<T> operator*() {
		return LocalCopy<T>{allocator().memory_device(), address()};
	}

  private:
	// @note Use a pointer to allow copy-assignment but enforce reference in
	// constructor
	Allocator *_allocator{};
	Address _address;
};

} // namespace rambock