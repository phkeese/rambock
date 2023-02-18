#include "base_layer.hpp"

rambock::layers::MemoryLayer::MemoryLayer(MemoryDevice &memory_device)
	: _memory_device{memory_device} {}
