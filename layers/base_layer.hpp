#pragma once

#include "../memory_device.hpp"

namespace rambock {
namespace layers {

/** Abstract Memory Layer
 * Requires an underlying MemoryDevice to work on.
 * Sub-classes need to implement read/write
 */
struct MemoryLayer : public MemoryDevice {
  protected:
	explicit MemoryLayer(MemoryDevice &memory_device);

	inline MemoryDevice &memory_device() const { return _memory_device; }

  private:
	MemoryDevice &_memory_device;
};
MemoryLayer::MemoryLayer(MemoryDevice &memory_device)
	: _memory_device{memory_device} {}

} // namespace layers
} // namespace rambock
