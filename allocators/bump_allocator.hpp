#pragma once

#include "../memory_device.hpp"
#include "base_allocator.hpp"

namespace rambock {
namespace allocators {

/** Unbound Bump Allocator
 * Allocates RAM by bumping a pointer, never reclaims memory
 * Will not raise an
 */
class BumpAllocator : public BaseAllocator {
	// steer clear of NULL and align to page boundaries
	Address _base = Address(32);
	Address _end;

  public:
	/** Constructor
	 * @param end the address just past the last addressable byte
	 */
	BumpAllocator(MemoryDevice &memory_device, Address end);

	Address allocate(Size count) override;
	Size free(Address address) override;
};

BumpAllocator::BumpAllocator(MemoryDevice &memory_device, const Address end)
	: BaseAllocator(memory_device)
	, _end{end} {}

Address BumpAllocator::allocate(Size count) {
	if (_base + count < _end) {
		Address address = _base;
		_base += count;
		return address;
	} else {
		return Address::null();
	}
}

Size BumpAllocator::free(Address address) {
	// do not reclaim any memory
	return 0;
}

} // namespace allocators
} // namespace rambock
