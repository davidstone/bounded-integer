// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/set.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>

#include <initializer_list>

namespace {
using namespace bounded::literal;

struct compares_address {
	constexpr explicit compares_address(int const & first, int const & second):
		m_first(first),
		m_second(second)
	{
	}
	constexpr explicit compares_address(std::pair<int const &, int const &> other):
		m_first(other.first),
		m_second(other.second)
	{
	}
	
	friend constexpr auto operator==(compares_address const lhs, compares_address const rhs) -> bool {
		return
			std::addressof(lhs.m_first) == std::addressof(rhs.m_first) and
			std::addressof(lhs.m_second) == std::addressof(rhs.m_second);
	}

	friend constexpr auto operator==(compares_address const lhs, std::pair<int const &, int const &> rhs) -> bool {
		return lhs == compares_address(rhs);
	}

private:
	int const & m_first;
	int const & m_second;
};

using init_list = std::initializer_list<compares_address>;

constexpr auto empty = containers::array<int, 0_bi>{};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, empty),
	init_list{}
));

constexpr auto one_one = containers::array{1};
constexpr auto one_two = containers::array{2};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, one_one),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(empty, one_two),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, one_one),
	init_list{compares_address(one_one[0_bi], one_one[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_one, one_two),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_two, one_one),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_two, one_two),
	init_list{compares_address(one_two[0_bi], one_two[0_bi])}
));

constexpr auto two_forward = containers::array{1, 2};
constexpr auto two_ones = containers::array{1, 1};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, two_forward),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, two_forward),
	init_list{compares_address(one_one[0_bi], two_forward[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, one_two),
	init_list{compares_address(two_forward[1_bi], one_two[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, one_one),
	init_list{compares_address(two_ones[0_bi], one_one[0_bi])}
));

static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, two_forward),
	init_list{
		compares_address(two_forward[0_bi], two_forward[0_bi]),
		compares_address(two_forward[1_bi], two_forward[1_bi])
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, two_ones),
	init_list{compares_address(two_forward[0_bi], two_ones[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, two_forward),
	init_list{compares_address(two_ones[0_bi], two_forward[0_bi])}
));

constexpr auto all_ones = containers::array{1, 1, 1, 1, 1, 1, 1};
constexpr auto prefix = containers::array{-4, -3, -2, 1, 1, 1};
constexpr auto suffix = containers::array{1, 1, 1, 2, 3, 4};
constexpr auto prefix_suffix = containers::array{-4, -3, -2, 1, 1, 1, 2, 3, 4};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, all_ones),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, all_ones),
	init_list{compares_address(one_one[0_bi], all_ones[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, one_one),
	init_list{compares_address(all_ones[0_bi], one_one[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, all_ones),
	init_list{
		compares_address(two_ones[0_bi], all_ones[0_bi]),
		compares_address(two_ones[1_bi], all_ones[1_bi])
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, two_ones),
	init_list{
		compares_address(all_ones[0_bi], two_ones[0_bi]),
		compares_address(all_ones[1_bi], two_ones[1_bi])
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, prefix),
	init_list{
		compares_address(all_ones[0_bi], prefix[3_bi]),
		compares_address(all_ones[1_bi], prefix[4_bi]),
		compares_address(all_ones[2_bi], prefix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix, all_ones),
	init_list{
		compares_address(prefix[3_bi], all_ones[0_bi]),
		compares_address(prefix[4_bi], all_ones[1_bi]),
		compares_address(prefix[5_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, suffix),
	init_list{
		compares_address(all_ones[0_bi], suffix[0_bi]),
		compares_address(all_ones[1_bi], suffix[1_bi]),
		compares_address(all_ones[2_bi], suffix[2_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(suffix, all_ones),
	init_list{
		compares_address(suffix[0_bi], all_ones[0_bi]),
		compares_address(suffix[1_bi], all_ones[1_bi]),
		compares_address(suffix[2_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, prefix_suffix),
	init_list{
		compares_address(all_ones[0_bi], prefix_suffix[3_bi]),
		compares_address(all_ones[1_bi], prefix_suffix[4_bi]),
		compares_address(all_ones[2_bi], prefix_suffix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix_suffix, all_ones),
	init_list{
		compares_address(prefix_suffix[3_bi], all_ones[0_bi]),
		compares_address(prefix_suffix[4_bi], all_ones[1_bi]),
		compares_address(prefix_suffix[5_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(suffix, prefix),
	init_list{
		compares_address(suffix[0_bi], prefix[3_bi]),
		compares_address(suffix[1_bi], prefix[4_bi]),
		compares_address(suffix[2_bi], prefix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix, suffix),
	init_list{
		compares_address(prefix[3_bi], suffix[0_bi]),
		compares_address(prefix[4_bi], suffix[1_bi]),
		compares_address(prefix[5_bi], suffix[2_bi]),
	}
));

constexpr auto squares = containers::array{1, 4, 9, 16, 25, 36, 49, 64};
constexpr auto evens = containers::array{2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

static_assert(containers::equal(
	containers::set_intersection_pair(squares, evens),
	init_list{
		compares_address(squares[1_bi], evens[1_bi]),
		compares_address(squares[3_bi], evens[7_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(evens, squares),
	init_list{
		compares_address(evens[1_bi], squares[1_bi]),
		compares_address(evens[7_bi], squares[3_bi]),
	}
));

} // namespace
