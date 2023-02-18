#pragma once

#include "rambock_common.hpp"

namespace rambock {

/** Abstract Memory Device
 * Allows for multiple layers of caching, paging, etc.
 */
struct MemoryDevice {
	virtual ~MemoryDevice() = default;

	/** Reads from a device into local storage
	 * @param from the address to read from
	 * @param to the address to copy the data to
	 * @param n the number of bytes to read
	 * @return pointer to beginning of local data
	 */
	virtual void *read(void *to, Address from, Size n) = 0;

	/** Writes to a device from local storage
	 * @param to the address to write to
	 * @param from the address to copy the data from
	 * @param n the number of bytes to write
	 * @return address of written data
	 */
	virtual Address write(Address to, const void *from, Size n) = 0;
};

} // namespace rambock
