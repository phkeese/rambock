#pragma once
#include "../layers/base_layer.hpp"
#include <chrono>
#include <cstdlib>
#include <thread>

namespace rambock {
namespace mocks {

template <size_t Nanoseconds>
struct MockSlowLayer : public layers::MemoryLayer {
	MockSlowLayer(MemoryDevice &memory_device)
		: MemoryLayer(memory_device) {}

	void *read(void *to, Address from, Size n) override {
		wait();
		return memory_device().read(to, from, n);
	}

	Address write(Address to, const void *from, Size n) override {
		wait();
		return memory_device().write(to, from, n);
	}

  private:
	inline void wait() {
		std::this_thread::sleep_for(std::chrono::nanoseconds(Nanoseconds));
	}
};

} // namespace mocks
} // namespace rambock
