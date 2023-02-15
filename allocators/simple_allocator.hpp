#pragma once

#include "../memory_allocator.hpp"

namespace rambock {

/** A simple memory allocator using a linked list of allocated sections
 * Does not try to prevent memory fragmentation and will always use the next
 * free block of RAM available.
 */
class SimpleAllocator : public MemoryAllocator {
	/** Information about an allocated block of memory
	 * Works as a linked list of blocks
	 */
	struct Header {
		// addresses of previous and next block
		Address previous, next;
		// range of allocated block
		Address begin, end;

		/** Calculate the address of the header
		 * @return address of the header
		 */
		inline Address address() const {
			// because we place the header directly before its data, we can
			// calculate the address of the header
			return begin - sizeof(*this);
		}

		/** Set begin to match an address for the header itself
		 * @param address the address of the header itself
		 * @return new begin value
		 */
		inline Address setAddress(Address address) {
			return begin = address + sizeof(*this);
		}

		/** Data segment size
		 * @return size in bytes
		 */
		inline Size size() const { return end - begin; }

		/** Set end to match begin and size
		 * @param size size of data after header
		 * @return new end value
		 */
		inline Address setSize(Size size) { return end = begin + size; }
	};

	// read the head from the array
	Header head() { return readHeader(0); }

	// address just past the last addressable byte
	Address m_end;
	inline Address end() const { return m_end; }

	// helpers to ease use of headers
	Header readHeader(Address from);
	void writeHeader(Address to, Header data);

  public:
	SimpleAllocator(MemoryDevice &memoryDevice, Address end);

	// setup data structures in memory for allocation
	void begin();

	virtual Address allocate(Size count) override;
	virtual Size free(Address address) override;
};

SimpleAllocator::SimpleAllocator(MemoryDevice &memoryDevice, Address end)
	: MemoryAllocator(memoryDevice)
	, m_end(end) {}

void SimpleAllocator::begin() {
	/** Special header to store data about the array
	 * previous points to itself
	 * next points to the end of memory
	 * begin points to the first available byte of memory
	 * end = begin
	 *
	 * This is done to allow allocate() to treat m_header as a normal
	 * header for a block of 0 bytes.
	 */
	Header head{};
	head.setAddress(0);
	head.setSize(0);
	head.previous = head.address();
	head.next = end();

	writeHeader(head.address(), head);
}

SimpleAllocator::Header SimpleAllocator::readHeader(Address from) {
	Header header;
	memory().read(from, &header, sizeof(header));
	return header;
}

void SimpleAllocator::writeHeader(Address to, Header data) {
	memory().write(to, &data, sizeof(data));
}

Address SimpleAllocator::allocate(Size count) {
#define ROUNDUP(c) (4 * (((c) + 4 - 1) / 4))
	// align to headers
	Size totalSize = sizeof(Header) + ROUNDUP(count);

	// Find next available part of memory by checking for each header, if we
	// can fit our data between it and the next. If not, try again at the next
	// header. Once we reach the end of RAM, return 0.
	Header current = head();
	while (true) {
		const Size available = current.next - ROUNDUP(current.end);

		// check if we can fit into the space between the current block's end
		// and the header for the next block
		if (totalSize <= available) {
			Address newAddress = ROUNDUP(current.end);

			Header newHeader{};
			newHeader.setAddress(newAddress);
			newHeader.setSize(count);

			// integrate into linked list
			// before: current <-> next
			// after:  current <-> newHeader <-> next

			newHeader.next = current.next;
			newHeader.previous = current.address();
			current.next = newAddress;

			// write out changes to headers
			writeHeader(newHeader.address(), newHeader);
			writeHeader(current.address(), current);

			// only touch next if it lies within bounds of memory
			if (newHeader.next < end()) {
				Header next = readHeader(newHeader.next);
				next.previous = newAddress;
				writeHeader(next.address(), next);
			}

			return newHeader.begin;
		} else if (current.next < end()) {
			// not at end, move along the list
			current = readHeader(current.next);
		} else {
			// end of list reached, no allocation possible
			return 0;
		}
	}
#undef ROUNDUP
}

Size SimpleAllocator::free(Address address) {
	// find the address of this block's header
	// done this way to let the Header struct decide about its size or optional
	// padding.
	Header header{};
	header.begin = address;
	// fill all necessary fields with actual data
	header = readHeader(header.address());

	// never free the head or out of bounds
	if (!header.address() || header.end > end()) {
		return 0;
	}

	// unlink from list
	Header previous = readHeader(header.previous);
	previous.next = header.next;
	writeHeader(previous.address(), previous);

	// only touch next if it lies within bounds of memory
	if (header.next < end()) {
		Header next = readHeader(header.next);
		next.previous = previous.address();
		writeHeader(next.address(), next);
	}

	return header.size();
}

} // namespace rambock
