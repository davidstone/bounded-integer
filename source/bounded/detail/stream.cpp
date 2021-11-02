// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/stream.hpp>

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>

#include <catch2/catch_test_macros.hpp>

#include <sstream>

namespace {

template<typename integer>
auto streaming_test(int const initial, int const expected) {
	auto value = integer(initial);
	std::stringstream in;
	in << value;
	CHECK(in.str() == std::to_string(initial));
	std::stringstream out;
	out << expected;
	out >> value;
	CHECK(value == expected);
}

TEST_CASE("stream small", "[stream]") {
	streaming_test<bounded::integer<0, 100>>(7, 0);
}

TEST_CASE("stream large", "[stream]") {
	constexpr auto large_initial = numeric_traits::max_value<int> / 3;
	constexpr auto large_final = -49;
	streaming_test<decltype(bounded::integer(0))>(large_initial, large_final);
}

} // namespace