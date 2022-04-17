#pragma once

#include <stddef.h>
#include <stdint.h>

namespace rambock {

// Abstract Memory Device
// Allows for mutliple layers of caching, paging, etc.
struct MemoryDevice {
	/** Reads from a device into local storage
	 * @param from the address to read from
	 * @param to the address to copy the data to
	 * @param count the number of bytes to read
	 * @return pointer to beginning of local data
	 */
	virtual uint8_t *read(uint32_t from, uint8_t *to, size_t count) = 0;

	/** Writes to a device from local storage
	 * @param to the address to write to
	 * @param from the address to copy the data from
	 * @param count the number of bytes to write
	 * @return pointer to beginning of local data
	 */
	virtual uint8_t *write(uint32_t to, uint8_t *from, size_t count) = 0;
};

} // namespace rambock
