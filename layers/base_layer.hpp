#pragma once

#include "../memory_device.hpp"

namespace rambock {
namespace layers {

/** Abstract Memory Layer
 * Requires an underlying memory device to work on.
 * Sub-classes need to implement read/write
 */
struct MemoryLayer : public MemoryDevice {
  protected:
	explicit MemoryLayer(MemoryDevice &memory);

	inline MemoryDevice &memory() const { return m_memory; }

  private:
	MemoryDevice &m_memory;
};
MemoryLayer::MemoryLayer(MemoryDevice &memory)
	: m_memory{memory} {}

} // namespace layers
} // namespace rambock
