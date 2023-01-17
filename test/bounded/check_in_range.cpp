// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <catch2/catch_test_macros.hpp>

export module bounded.check_in_range_test;

import bounded.check_in_range;
import bounded.integer;

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