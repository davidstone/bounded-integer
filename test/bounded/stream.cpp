// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <catch2/catch_test_macros.hpp>

export module bounded.stream_test;

import bounded.integer;
import bounded.comparison;
import bounded.stream;
import numeric_traits;
import std_module;

namespace {

template<typename integer>
auto streaming_test(auto const initial, auto const expected) {
	auto value = integer(initial);
	std::stringstream in;
	in << value;
	CHECK(in.str() == std::to_string(static_cast<int>(initial)));
	std::stringstream out;
	out << expected;
	out >> value;
	CHECK(value == expected);
}

TEST_CASE("stream small", "[stream]") {
	streaming_test<bounded::integer<0, 100>>(bounded::constant<7>, bounded::constant<0>);
}

TEST_CASE("stream large", "[stream]") {
	constexpr auto large_initial = bounded::constant<numeric_traits::max_value<int> / 3>;
	constexpr auto large_final = bounded::constant<-49>;
	streaming_test<decltype(bounded::integer(0))>(large_initial, large_final);
}

} // namespace