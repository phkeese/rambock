#pragma once

#include "../memory_allocator.hpp"
#include "../memory_device.hpp"

namespace rambock {

/** Unbound Bump Allocator
 * Allocates RAM by bumping a pointer, never reclaims memory
 * Will not raise an
 */
class BumpAllocator : public MemoryAllocator {

	uint32_t m_base = 32; // steer clear of NULL and align to page boundaries
	uint32_t m_end;

  public:
	/** Constructor
	 * @param end the address just past the last addressable byte
	 */
	BumpAllocator(uint32_t end);

	virtual uint32_t allocate(size_t count) override;
	virtual size_t free(uint32_t address) override;
};

BumpAllocator::BumpAllocator(uint32_t end) : m_end(end) {}

uint32_t BumpAllocator::allocate(size_t count) {
	if (m_base + count < m_end) {
		uint32_t address = m_base;
		m_base += count;
		return address;
	} else {
		return 0;
	}
}

size_t BumpAllocator::free(uint32_t address) {
	// do not reclaim any memory
	return 0;
}

} // namespace rambock
