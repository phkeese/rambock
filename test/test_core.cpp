#include "../rambock_common.hpp"
#include <catch2/catch_all.hpp>

using namespace rambock;

TEST_CASE("test address semantics", "[core]") {
	Address a = Address::null();
	Address b = Address(1);
	Address c = Address(2);

	SECTION("address supports addition") {
		REQUIRE(a + 1 == b);
		REQUIRE(b - 1 == a);
		REQUIRE(b + 1 == c);
	}

	SECTION("address difference is bytes") { REQUIRE(c - a == 2); }

	SECTION("addition assignment") {
		Address d = a;
		d += 1;
		REQUIRE(d == b);
	}
}