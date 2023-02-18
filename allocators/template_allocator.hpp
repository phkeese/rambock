#pragma once

#include "../external_ptr.hpp"
#include "base_allocator.hpp"

namespace rambock {
namespace allocators {

struct TemplateAllocator {
	explicit TemplateAllocator(BaseAllocator &allocator)
		: _allocator(allocator) {}

	inline BaseAllocator &allocator() const { return _allocator; }

	template <class T, class... Args>
	external_ptr<T> make_external(Args &&...args);

  private:
	BaseAllocator &_allocator;
};

template <class T, class... Args>
external_ptr<T> TemplateAllocator::make_external(Args &&...args) {
	const Size allocation_size = external_ptr<T>::allocation_size;

	// Request memory before constructing object
	Address address = allocator().allocate(allocation_size);
	if (!address) {
		return external_ptr<T>{allocator(), Address::null()};
	}

	// Construct object
	// @note T must be TriviallyCopyable
	T value{args...};

	LocalCopy<T> local_copy{allocator().memory_device(), address, value};
	return external_ptr<T>{allocator(), address};
}

} // namespace allocators
} // namespace rambock