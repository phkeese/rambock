#pragma once
#include "memory_device.hpp"
#include <cstddef>
#include <cstdlib>

#ifdef STRICT_CHECKS
#include <type_traits>
#define CHECK_CONSTRAINTS(T)                       \
	static_assert(std::is_trivially_copyable<T>(), \
				  "T must be trivially copyable");
#else
#define CHECK_CONSTRAINTS(T)
#endif

namespace rambock {

namespace allocators {
struct TemplateAllocator;
}
template <typename T> struct external_ptr;

template <typename T> struct LocalCopy {
	CHECK_CONSTRAINTS(T);

	LocalCopy(MemoryDevice &memory_device, Address address);
	~LocalCopy();

	inline MemoryDevice &memory_device() const { return *_memory_device; }
	inline Address address() const { return _address; }
	inline T *local_address() const { return _frame.local_address; }
	inline bool is_first() const { return local_address() == &_frame.value; }

	inline T *operator->() { return local_address(); }
	inline const T *operator->() const { return local_address(); }
	LocalCopy &operator=(const T &value);
	inline operator T() { return *local_address(); }

  private:
	struct ExternalFrame;

	LocalCopy(MemoryDevice &memory_device, Address address, const T &value);
	ExternalFrame read_frame() const;
	void write_frame() const;

	MemoryDevice *_memory_device;
	Address _address;
	ExternalFrame _frame;

	friend struct rambock::allocators::TemplateAllocator;
	friend struct rambock::external_ptr<T>;

	struct ExternalFrame {
		T *local_address;
		T value;
	};
};

template <typename T>
rambock::LocalCopy<T>::LocalCopy(MemoryDevice &memory_device, Address address)
	: _memory_device{&memory_device}
	, _address{address}
	, _frame{read_frame()} {
	if (_frame.local_address == nullptr) {
		_frame.local_address = &_frame.value;
	}
}

template <typename T> rambock::LocalCopy<T>::~LocalCopy() {
	if (is_first()) {
		write_frame();
	}
}

template <typename T>
rambock::LocalCopy<T>::LocalCopy(MemoryDevice &memory_device,
								 Address address,
								 const T &value)
	: _memory_device{&memory_device}
	, _address(address)
	, _frame{&_frame.value, value} {}

template <typename T> LocalCopy<T> &LocalCopy<T>::operator=(const T &value) {
	*local_address() = value;
	return *this;
}

template <typename T>
typename LocalCopy<T>::ExternalFrame LocalCopy<T>::read_frame() const {
	ExternalFrame frame{};
	memory_device().read(address(), &frame, sizeof(frame));
	return frame;
}

template <typename T> void LocalCopy<T>::write_frame() const {
	ExternalFrame frame{nullptr, _frame.value};
	memory_device().write(address(), &frame, sizeof(frame));
}

} // namespace rambock
