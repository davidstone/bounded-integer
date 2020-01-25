// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/filter_iterator.hpp>

#include <containers/array/array.hpp>

namespace {

using namespace bounded::literal;

constexpr auto check_filter() {
	constexpr auto source = containers::array{1_bi, 2_bi, 3_bi, 2_bi, 4_bi, 5_bi, 6_bi, 8_bi};
	constexpr auto expected = containers::array{2_bi, 2_bi, 4_bi, 6_bi, 8_bi};
	auto const filtered = containers::filter(source, [](auto const integer) { return integer % 2_bi == 0_bi; });
	return containers::equal(filtered, expected);
}

static_assert(check_filter());

}	// namespace
