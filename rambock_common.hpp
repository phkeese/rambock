#pragma once
#include <stdint.h>

/** Common definitions and types used across rambock
 */
namespace rambock {

/** Necessary because size_t only covers the architecture the code is running on
 * On a system with < 64k RAM, this might be only 16 bit wide.
 * Rambock supports larger sizes, so it must ensure that those fit.
 */
using Size = uint32_t;

struct Address {
	using value_type = uint32_t;
	constexpr Address()
		: Address(0) {}
	constexpr explicit Address(const value_type value)
		: value{value} {}

	template <typename T> constexpr operator T() const { return T(value); }

	inline Address &operator+=(const Size n) {
		value += n;
		return *this;
	}
	inline Address &operator-=(const Size n) {
		value -= n;
		return *this;
	}

	template <typename T> constexpr Address operator+(const T &n) const {
		return Address(value + n);
	}
	template <typename T> constexpr Address operator-(const T &n) const {
		return Address(value + n);
	}
	constexpr Size operator-(const Address &other) const {
		return value - other.value;
	}

	constexpr bool operator<(const Address &other) const {
		return value < other.value;
	}
	constexpr bool operator>(const Address &other) const {
		return value > other.value;
	}
	constexpr bool operator<=(const Address &other) const {
		return value <= other.value;
	}
	constexpr bool operator>=(const Address &other) const {
		return value >= other.value;
	}
	constexpr bool operator==(const Address &rhs) const {
		return value == rhs.value;
	}
	constexpr bool operator!=(const Address &rhs) const {
		return !(rhs == *this);
	}

	value_type value = 0;

	static constexpr Address null() { return Address(0); };
};

} // namespace rambock
