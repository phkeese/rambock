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

	Address m_base = Address(32); // steer clear of NULL and align to page boundaries
	Address m_end;

  public:
	/** Constructor
	 * @param end the address just past the last addressable byte
	 */
	BumpAllocator(MemoryDevice &memoryDevice, Address end);

	virtual Address allocate(Size count) override;
	virtual Size free(Address address) override;
};

BumpAllocator::BumpAllocator(MemoryDevice &memoryDevice, const Address end)
	: BaseAllocator(memoryDevice)
	, m_end{end} {}

Address BumpAllocator::allocate(Size count) {
	if (m_base + count < m_end) {
		Address address = m_base;
		m_base += count;
		return address;
	} else {
		return rambock::null;
	}
}

Size BumpAllocator::free(Address address) {
	// do not reclaim any memory
	return 0;
}

} // namespace allocators
} // namespace rambock
