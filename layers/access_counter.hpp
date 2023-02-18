#pragma once
#include "base_layer.hpp"

namespace rambock {
namespace layers {

struct AccessCounter : public MemoryLayer {
	explicit AccessCounter(MemoryDevice &memory_device);

	void *read(void *to, Address from, Size count) override;
	Address write(Address to, const void *from, Size count) override;

	inline int reads() const { return _reads; }
	inline int writes() const { return _writes; }
	inline void reset() { _reads = _writes = 0; }

  private:
	int _reads, _writes;
};

} // namespace layers
} // namespace rambock