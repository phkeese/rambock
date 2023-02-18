#pragma once

#include "allocators/base_allocator.hpp"
#include "local_copy.hpp"
#include "memory_device.hpp"

namespace rambock {

template <typename T> struct external_ptr {
	using Allocator = rambock::allocators::BaseAllocator;
	static constexpr Size allocation_size =
		sizeof(typename LocalCopy<T>::ExternalFrame);

	constexpr explicit external_ptr(Allocator &allocator);
	constexpr external_ptr(Allocator &allocator, const Address address);
	constexpr external_ptr(const external_ptr &) = default;
	constexpr external_ptr(external_ptr &&) noexcept = default;
	external_ptr &operator=(const external_ptr &) = default;
	external_ptr &operator=(external_ptr &&) noexcept = default;

	inline Allocator &allocator() const { return *_allocator; }
	inline Address address() const { return _address; }

	inline LocalCopy<T> operator->() const { return this->operator*(); }
	inline LocalCopy<T> operator*() const {
		return LocalCopy<T>{allocator().memory_device(), address()};
	}
	inline external_ptr &operator++() { *this = *this + 1; }
	inline external_ptr &operator--() { *this = *this - 1; }
	inline external_ptr operator++(int) const { return *this + 1; }
	inline external_ptr operator--(int) const { return *this - 1; }
	inline external_ptr operator+(size_t i) const;
	inline external_ptr operator-(size_t i) const;
	inline LocalCopy<T> operator[](size_t i) const { return *(*this + i); }
	inline void free() const { allocator().free(address()); }

  private:
	// @note Use a pointer to allow copy-assignment but enforce reference in
	// constructor
	Allocator *_allocator{};
	Address _address;
};

template <typename T>
external_ptr<T> external_ptr<T>::operator+(size_t i) const {
	return external_ptr{allocator(), address() + i * allocation_size};
}

template <typename T>
external_ptr<T> external_ptr<T>::operator-(size_t i) const {
	return external_ptr{allocator(), address() - i * allocation_size};
}

template <typename T>
constexpr external_ptr<T>::external_ptr(external_ptr::Allocator &allocator)
	: _allocator{&allocator}
	, _address(Address::null()) {}

template <typename T>
constexpr external_ptr<T>::external_ptr(external_ptr::Allocator &allocator,
										const Address address)
	: _allocator{&allocator}
	, _address{address} {}

} // namespace rambock