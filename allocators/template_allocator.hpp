#pragma once

#include "../external_ptr.hpp"
#include "base_allocator.hpp"

namespace rambock {
namespace allocators {

struct TemplateAllocator {
	TemplateAllocator(BaseAllocator &allocator)
		: _allocator(allocator) {}

	inline BaseAllocator &allocator() const { return _allocator; }

	template <class T, class... Args>
	external_ptr<T> make_external(Args &&...args);

  private:
	BaseAllocator &_allocator;
};

template <class T, class... Args>
external_ptr<T> TemplateAllocator::make_external(Args &&...args) {
	// Request memory before constructing object
	Address address = allocator().allocate(sizeof(T));
	if (!address) {
		return external_ptr<T>{allocator(), Address::null()};
	}

	// Construct object
	// @note T must be TriviallyCopyable
	T value{args...};

	// Copy to external memory
	allocator().memoryDevice().write(address, &value, sizeof(value));

	return external_ptr<T>{allocator(), address};
}

} // namespace allocators
} // namespace rambock