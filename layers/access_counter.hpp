#include "base_layer.hpp"

namespace rambock {
namespace layers {

struct AccessCounter : public MemoryLayer {
	explicit AccessCounter(MemoryDevice &memory_device);

	void *read(Address from, void *to, Size count) override;
	Address write(Address to, const void *from, Size count) override;

	inline int reads() const { return _reads; }
	inline int writes() const { return _writes; }
	inline void reset() { _reads = _writes = 0; }

  private:
	int _reads, _writes;
};

void *AccessCounter::read(Address from, void *to, Size count) {
	_reads++;
	return memory_device().read(from, to, count);
}

Address AccessCounter::write(Address to, const void *from, Size count) {
	_writes++;
	return memory_device().write(to, from, count);
}

AccessCounter::AccessCounter(MemoryDevice &memory_device)
	: MemoryLayer(memory_device)
	, _reads{0}
	, _writes{0} {}

} // namespace layers
} // namespace rambock