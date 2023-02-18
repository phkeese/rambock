#include "access_counter.hpp"

void *rambock::layers::AccessCounter::read(void *to, Address from, Size count) {
	_reads++;
	return memory_device().read(to, from, count);
}
rambock::Address rambock::layers::AccessCounter::write(Address to,
													   const void *from,
													   Size count) {
	_writes++;
	return memory_device().write(to, from, count);
}
rambock::layers::AccessCounter::AccessCounter(MemoryDevice &memory_device)
	: MemoryLayer(memory_device)
	, _reads{0}
	, _writes{0} {}