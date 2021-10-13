// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/check_in_range.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE("check_in_range", "[check_in_range]") {
	constexpr auto minimum = bounded::constant<0>;
	constexpr auto maximum = bounded::constant<10>;
	CHECK_THROWS(bounded::check_in_range(
		bounded::integer<-20, 20>(bounded::constant<20>),
		minimum,
		maximum
	));
	CHECK_THROWS(bounded::check_in_range(
		bounded::integer<-6, 6>(bounded::constant<-6>),
		minimum,
		maximum
	));
}

} // namespace