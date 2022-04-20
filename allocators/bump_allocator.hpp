#pragma once

#include "../memory_allocator.hpp"
#include "../memory_device.hpp"

namespace rambock {

/** Unbound Bump Allocator
 * Allocates RAM by bumping a pointer, never reclaims memory
 * Will not raise an
 */
class BumpAllocator : public MemoryAllocator {

	Address m_base = 32; // steer clear of NULL and align to page boundaries
	Address m_end;

  public:
	/** Constructor
	 * @param end the address just past the last addressable byte
	 */
	BumpAllocator(Address end);

	virtual Address allocate(Size count) override;
	virtual Size free(Address address) override;
};

BumpAllocator::BumpAllocator(Address end) : m_end(end) {}

Address BumpAllocator::allocate(Size count) {
	if (m_base + count < m_end) {
		Address address = m_base;
		m_base += count;
		return address;
	} else {
		return 0;
	}
}

Size BumpAllocator::free(Address address) {
	// do not reclaim any memory
	return 0;
}

} // namespace rambock
