// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.keyed_binary_search;

import containers.algorithms.binary_search;
import containers.associative_container;
import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.map_value_type;

import bounded;
import std_module;

namespace containers {

export constexpr auto keyed_lower_bound(associative_range auto && map, auto && key) {
	return containers::lower_bound(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

export constexpr auto keyed_upper_bound(associative_range auto && map, auto && key) {
	return containers::upper_bound(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

export constexpr auto keyed_equal_range(associative_range auto && map, auto && key) {
	return containers::equal_range(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

} // namespace containers

using namespace bounded::literal;

struct compare_t {
	static constexpr auto operator()(containers::map_value_type<int, int> const lhs, int const rhs) -> bool {
		return lhs.key < rhs;
	}
	static constexpr auto operator()(int const lhs, containers::map_value_type<int, int> const rhs) -> bool {
		return lhs < rhs.key;
	}
	static constexpr auto operator()(int const lhs, int const rhs) -> bool {
		return lhs < rhs;
	}
};

template<auto size_>
struct map_type {
	using key_type = int;
	using mapped_type = int;

	map_type() = default;
	template<std::size_t size> requires(bounded::constant<size> == size_)
	explicit constexpr map_type(containers::c_array<containers::map_value_type<int, int>, size> && source):
		m_data(to_array(source))
	{
	}

	constexpr auto begin() const {
		return containers::begin(m_data);
	}
	static constexpr auto size() {
		return size_;
	}
	static constexpr auto compare() {
		return compare_t();
	}

	containers::array<containers::map_value_type<int, int>, size_> m_data;
};

constexpr auto zero = map_type<0_bi>();
static_assert(containers::keyed_lower_bound(zero, 0) == containers::end(zero));
static_assert(containers::keyed_upper_bound(zero, 0) == containers::end(zero));

constexpr auto one = map_type<1_bi>({{1, 0}});
static_assert(containers::keyed_lower_bound(one, 0) == containers::begin(one));
static_assert(containers::keyed_lower_bound(one, 1) == containers::begin(one));
static_assert(containers::keyed_lower_bound(one, 2) == containers::end(one));
static_assert(containers::keyed_upper_bound(one, 0) == containers::begin(one));
static_assert(containers::keyed_upper_bound(one, 1) == containers::end(one));
static_assert(containers::keyed_upper_bound(one, 2) == containers::end(one));

constexpr auto two = map_type<2_bi>({{1, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(two, 0) == containers::begin(two) + 0_bi);
static_assert(containers::keyed_lower_bound(two, 1) == containers::begin(two) + 0_bi);
static_assert(containers::keyed_lower_bound(two, 2) == containers::begin(two) + 1_bi);
static_assert(containers::keyed_lower_bound(two, 3) == containers::end(two));
static_assert(containers::keyed_upper_bound(two, 0) == containers::begin(two) + 0_bi);
static_assert(containers::keyed_upper_bound(two, 1) == containers::begin(two) + 1_bi);
static_assert(containers::keyed_upper_bound(two, 2) == containers::begin(two) + 2_bi);
static_assert(containers::keyed_upper_bound(two, 3) == containers::end(two));

constexpr auto three = map_type<3_bi>({{1, 0}, {2, 0}, {3, 0}});
static_assert(containers::keyed_lower_bound(three, 0) == containers::begin(three) + 0_bi);
static_assert(containers::keyed_lower_bound(three, 1) == containers::begin(three) + 0_bi);
static_assert(containers::keyed_lower_bound(three, 2) == containers::begin(three) + 1_bi);
static_assert(containers::keyed_lower_bound(three, 3) == containers::begin(three) + 2_bi);
static_assert(containers::keyed_lower_bound(three, 4) == containers::end(three));
static_assert(containers::keyed_upper_bound(three, 0) == containers::begin(three) + 0_bi);
static_assert(containers::keyed_upper_bound(three, 1) == containers::begin(three) + 1_bi);
static_assert(containers::keyed_upper_bound(three, 2) == containers::begin(three) + 2_bi);
static_assert(containers::keyed_upper_bound(three, 3) == containers::begin(three) + 3_bi);
static_assert(containers::keyed_upper_bound(three, 4) == containers::end(three));

constexpr auto two_duplicates = map_type<2_bi>({{1, 0}, {1, 0}});
static_assert(containers::keyed_lower_bound(two_duplicates, 0) == containers::begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(two_duplicates, 1) == containers::begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(two_duplicates, 2) == containers::end(two_duplicates));
static_assert(containers::keyed_upper_bound(two_duplicates, 0) == containers::begin(two_duplicates) + 0_bi);
static_assert(containers::keyed_upper_bound(two_duplicates, 1) == containers::end(two_duplicates));
static_assert(containers::keyed_upper_bound(two_duplicates, 2) == containers::end(two_duplicates));

constexpr auto three_duplicates = map_type<3_bi>({{1, 0}, {1, 0}, {1, 0}});
static_assert(containers::keyed_lower_bound(three_duplicates, 0) == containers::begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(three_duplicates, 1) == containers::begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_lower_bound(three_duplicates, 2) == containers::end(three_duplicates));
static_assert(containers::keyed_upper_bound(three_duplicates, 0) == containers::begin(three_duplicates) + 0_bi);
static_assert(containers::keyed_upper_bound(three_duplicates, 1) == containers::end(three_duplicates));
static_assert(containers::keyed_upper_bound(three_duplicates, 2) == containers::end(three_duplicates));

constexpr auto three_two_duplicates_first = map_type<3_bi>({{1, 0}, {1, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 0) == containers::begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 1) == containers::begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 2) == containers::begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_first, 3) == containers::end(three_two_duplicates_first));
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 0) == containers::begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 1) == containers::begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 2) == containers::begin(three_two_duplicates_first) + 3_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_first, 3) == containers::end(three_two_duplicates_first));

constexpr auto three_two_duplicates_last = map_type<3_bi>({{1, 0}, {2, 0}, {2, 0}});
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 0) == containers::begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 1) == containers::begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 2) == containers::begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::keyed_lower_bound(three_two_duplicates_last, 3) == containers::end(three_two_duplicates_last));
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 0) == containers::begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 1) == containers::begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 2) == containers::begin(three_two_duplicates_last) + 3_bi);
static_assert(containers::keyed_upper_bound(three_two_duplicates_last, 3) == containers::end(three_two_duplicates_last));