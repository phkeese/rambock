#pragma once
#include "memory_device.hpp"
#include <cstddef>
#include <cstdlib>

#ifdef STRICT_CHECKS
#include <type_traits>
#define CHECK_CONSTRAINTS(T) static_assert(std::is_trivially_copyable<T>(), "T must be trivially copyable");
#else
#define CHECK_CONSTRAINTS(T)
#endif

namespace rambock {

namespace allocators {
struct TemplateAllocator;
}

template <typename T> struct LocalCopy {
	CHECK_CONSTRAINTS(T);

	LocalCopy(MemoryDevice &memoryDevice, Address address);
	~LocalCopy();

	inline MemoryDevice &memoryDevice() const { return *_memoryDevice; }
	inline Address address() const { return _address; }
	inline T *local_address() const { return _local_address; }
	inline bool is_first() const { return _is_first; }

	inline T *operator->() { return local_address(); }
	inline const T *operator->() const { return local_address(); }
	LocalCopy &operator=(const T &value);
	inline operator T() {return *local_address();}

	struct ExternalFrame {
		T *local_address;
		T value;
	};

  private:
	LocalCopy(MemoryDevice &memoryDevice, Address address, const T &value);
	T *read_local_address() const;
	void read_exteral_value();
	void write_exteral_value();

	MemoryDevice *_memoryDevice;
	Address _address;
	T *_local_address;
	bool _is_first;
	T _value;

	static constexpr Size local_address_offset() {
		return offsetof(ExternalFrame, local_address);
	};
	static constexpr Size value_offset() {
		return offsetof(ExternalFrame, value);
	}

	friend struct rambock::allocators::TemplateAllocator;
};

template <typename T>
rambock::LocalCopy<T>::LocalCopy(MemoryDevice &memoryDevice, Address address)
	: _memoryDevice{&memoryDevice}
	, _address{address}
	, _local_address(read_local_address())
	, _is_first(local_address() == nullptr) {
	if (is_first()) {
		read_exteral_value();
	}
}

template <typename T> rambock::LocalCopy<T>::~LocalCopy() {
	if (is_first()) {
		write_exteral_value();
	}
}

template <typename T>
rambock::LocalCopy<T>::LocalCopy(MemoryDevice &memoryDevice,
								 Address address,
								 const T &value)
	: _memoryDevice{&memoryDevice}
	, _address(address)
	, _local_address(&_value)
	, _is_first(true)
	, _value{value} {}

template <typename T> void rambock::LocalCopy<T>::read_exteral_value() {
	// Mark ourselves as first copy by writing our address
	_local_address = &_value;
	memoryDevice().write(address() + local_address_offset(),
						 &_local_address,
						 sizeof(_local_address));

	// Copy external value
	memoryDevice().read(
		address() + value_offset(), _local_address, sizeof(*_local_address));
}

template <typename T> void rambock::LocalCopy<T>::write_exteral_value() {
	// Write back external value
	memoryDevice().write(
		address() + value_offset(), _local_address, sizeof(*_local_address));

	// Remove reference to ourselves
	_local_address = nullptr;
	memoryDevice().write(address() + local_address_offset(),
						 &_local_address,
						 sizeof(_local_address));
}

template <typename T> T *rambock::LocalCopy<T>::read_local_address() const {
	T *local_address = nullptr;
	memoryDevice().read(address() + local_address_offset(),
						&local_address,
						sizeof(local_address));
	return local_address;
}

template <typename T> LocalCopy<T> &LocalCopy<T>::operator=(const T &value) {
	*local_address() = value;
	return *this;
}

} // namespace rambock
