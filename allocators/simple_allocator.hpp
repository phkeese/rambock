#pragma once

#include "base_allocator.hpp"

namespace rambock {
namespace allocators {
/** A simple memory allocator using a linked list of allocated sections
 * Does not try to prevent memory fragmentation and will always use the next
 * free block of RAM available.
 */
class SimpleAllocator : public BaseAllocator {
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
		inline Address set_address(Address address) {
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
		inline Address set_size(Size size) { return end = begin + size; }
	};

	// read the head from the array
	Header head() { return read_header(Address(0)); }

	// address just past the last addressable byte
	Address _end;
	inline Address end() const { return _end; }

	// helpers to ease use of headers
	Header read_header(Address from);
	void write_header(Address to, Header data);

	Size _free_bytes;

  public:
	SimpleAllocator(MemoryDevice &memory_device, Address end);

	// setup data structures in memory for allocation
	void begin();

	Address allocate(Size count) override;
	Size free(Address address) override;
	virtual Size get_free_bytes() const override;
};

SimpleAllocator::SimpleAllocator(MemoryDevice &memory_device, Address end)
	: BaseAllocator(memory_device)
	, _end(end)
	, _free_bytes{end} {}

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
	head.set_address(Address(0));
	head.set_size(0);
	head.previous = head.address();
	head.next = end();

	write_header(head.address(), head);
}

SimpleAllocator::Header SimpleAllocator::read_header(Address from) {
	Header header;
	memory_device().read(from, &header, sizeof(header));
	return header;
}

void SimpleAllocator::write_header(Address to, Header data) {
	memory_device().write(to, &data, sizeof(data));
}

Address SimpleAllocator::allocate(Size count) {
#define ROUNDUP(c) (4 * (((c) + 4 - 1) / 4))
	// align to headers
	Size total_size = sizeof(Header) + ROUNDUP(count);

	// Find next available part of memory by checking for each header, if we
	// can fit our data between it and the next. If not, try again at the next
	// header. Once we reach the end of RAM, return 0.
	Header current = head();
	while (true) {
		const Address end_address = Address(ROUNDUP(current.end.value));
		const Size available = current.next - end_address;

		// check if we can fit into the space between the current block's end
		// and the header for the next block
		if (total_size <= available) {
			Address new_address = end_address;

			Header new_header{};
			new_header.set_address(new_address);
			new_header.set_size(count);

			// integrate into linked list
			// before: current <-> next
			// after:  current <-> new_header <-> next

			new_header.next = current.next;
			new_header.previous = current.address();
			current.next = new_address;

			// write out changes to headers
			write_header(new_header.address(), new_header);
			write_header(current.address(), current);

			// only touch next if it lies within bounds of memory
			if (new_header.next < end()) {
				Header next = read_header(new_header.next);
				next.previous = new_address;
				write_header(next.address(), next);
			}

			_free_bytes -= total_size;

			return new_header.begin;
		} else if (current.next < end()) {
			// not at end, move along the list
			current = read_header(current.next);
		} else {
			// end of list reached, no allocation possible
			return Address::null();
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
	header = read_header(header.address());

	// never free the head or out of bounds
	if (!header.address() || header.end > end()) {
		return 0;
	}

	// unlink from list
	Header previous = read_header(header.previous);
	previous.next = header.next;
	write_header(previous.address(), previous);

	// only touch next if it lies within bounds of memory
	if (header.next < end()) {
		Header next = read_header(header.next);
		next.previous = previous.address();
		write_header(next.address(), next);
	}

	_free_bytes += header.size() + sizeof(header);
	return header.size();
}

Size SimpleAllocator::get_free_bytes() const { return _free_bytes; }

} // namespace allocators
} // namespace rambock
