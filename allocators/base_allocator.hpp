#pragma once

#include "../memory_device.hpp"

namespace rambock {

namespace allocators {

/** Abstract Memory Allocator
 * Allows for allocation and de-allocation of external memory
 * Only manages memory, does not interoperate with local copies, locks, etc.
 * Probably needs a MemoryDevice to be useful, but the interface alone is not
 * aware of anything other than external addresses.
 */
struct BaseAllocator {
	explicit BaseAllocator(MemoryDevice &memory_device)
		: _memory_device{memory_device} {}

	virtual ~BaseAllocator() = default;

	/** Allocates external memory of at least count bytes
	 * @param count the number of bytes to allocate
	 * @return the address at which count bytes lie
	 * returns 0 in case of allocation error
	 */
	virtual Address allocate(Size count) = 0;

	/** Frees external memory at an address
	 * @param address the beginning of the external memory to be freed
	 * @return the number of bytes freed (may be larger than allocated)
	 */
	virtual Size free(Address address) = 0;

	/** Get memory device for this allocator
	 * @return Memory device this allocator operates on
	 */
	inline MemoryDevice &memory_device() const { return _memory_device; }

	/**
	 * @brief Get number of unallocated bytes
	 * @return Number of unallocated bytes
	 */
	virtual Size get_free_bytes() const = 0;

  private:
	MemoryDevice &_memory_device;
};

} // namespace allocators
} // namespace rambock
