#pragma once

#include "rambock_common.hpp"

namespace rambock {

/** Abstract Memory Device
 * Allows for mutliple layers of caching, paging, etc.
 */
struct MemoryDevice {
	virtual ~MemoryDevice(){};

	/** Reads from a device into local storage
	 * @param from the address to read from
	 * @param to the address to copy the data to
	 * @param count the number of bytes to read
	 * @return pointer to beginning of local data
	 */
	virtual void *read(Address from, void *to, Size count) = 0;

	/** Writes to a device from local storage
	 * @param to the address to write to
	 * @param from the address to copy the data from
	 * @param count the number of bytes to write
	 * @return address of written data
	 */
	virtual Address write(Address to, const void *from, Size count) = 0;
};

} // namespace rambock
