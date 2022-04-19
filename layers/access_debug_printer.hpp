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
	explicit AccessDebugPrinter(MemoryDevice &memory);

	virtual void *read(uint32_t from, void *to, size_t count) override;
	virtual uint32_t write(uint32_t to, const void *from,
						   size_t count) override;
};

AccessDebugPrinter::AccessDebugPrinter(MemoryDevice &memory)
	: MemoryLayer(memory) {}

void *AccessDebugPrinter::read(uint32_t from, void *to, size_t count) {
	char buf[64];
	sprintf(buf, "read %lu from %06lx to %p", (unsigned long)count,
			(unsigned long)from, to);
	Serial.println(buf);

	return memory().read(from, to, count);
}

uint32_t AccessDebugPrinter::write(uint32_t to, const void *from,
								   size_t count) {
	char buf[64];
	sprintf(buf, "write %lu to %06lx from %p", (unsigned long)count,
			(unsigned long)to, from);

	Serial.println(buf);

	return memory().write(to, from, count);
}

} // namespace rambock
