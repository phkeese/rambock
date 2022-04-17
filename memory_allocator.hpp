#pragma once

#include "memory_device.hpp"

namespace rambock {

/** Abstract Memory Allocator
 * Allows for allocation and deallocation of external memory
 * Only manages memory, does not interoperate with local copies, locks, etc.
 * Probably needs a MemoryDevice to be useful, but the interface alone is not
 * aware of anything other than external addresses.
 */
struct MemoryAllocator {
	/** Allocates external memory of at least count bytes
	 * @param count the number of bytes to allocate
	 * @return the address at which count bytes lie
	 */
	virtual uint32_t allocate(size_t count) = 0;

	/** Frees external memory at an address
	 * @param address the beginning of the external memory to be freed
	 * @return the number of bytes freed (may be larger than allocated)
	 */
	virtual size_t free(uint32_t address) = 0;
};

} // namespace rambock
