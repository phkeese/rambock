#pragma once

#include "../memory_device.hpp"
#include "../memory_layer.hpp"
#include <Arduino.h>

namespace rambock {

/** Access Debug Printer
 * Prints every access to the underlying device to the serial port
 */
class AccessDebugPrinter : public MemoryLayer {
  public:
	AccessDebugPrinter(MemoryDevice &memory);

	virtual uint8_t *read(uint32_t from, uint8_t *to, size_t count) override;
	virtual uint32_t write(uint32_t to, const uint8_t *from,
						   size_t count) override;
};

AccessDebugPrinter::AccessDebugPrinter(MemoryDevice &memory)
	: MemoryLayer(memory) {}

uint8_t *AccessDebugPrinter::read(uint32_t from, uint8_t *to, size_t count) {
	char buf[64];
	sprintf(buf, "read %lu from %06lx to %p", (unsigned long)count, from, to);
	Serial.println(buf);

	return memory().read(from, to, count);
}

uint32_t AccessDebugPrinter::write(uint32_t to, const uint8_t *from,
								   size_t count) {
	char buf[64];
	sprintf(buf, "write %lu to %06lx from %p", (unsigned long)count, to, from);

	Serial.println(buf);

	return memory().write(to, from, count);
}

} // namespace rambock
