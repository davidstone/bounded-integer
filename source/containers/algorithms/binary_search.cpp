// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.binary_search;

import containers.algorithms.partition;
import containers.array;
import containers.begin_end;
import containers.range;
import containers.range_view;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

struct lower_bound_t {
	static constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return cmp(element, value); });
	}
	static constexpr auto operator()(range auto && sorted, auto const & value) {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less());
	}
};
export constexpr auto lower_bound = lower_bound_t();

struct upper_bound_t {
	static constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return !cmp(value, element); });
	}
	static constexpr auto operator()(range auto && sorted, auto const & value) {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less());
	}
};
export constexpr auto upper_bound = upper_bound_t();

// TODO: This can be implemented more efficiently
struct equal_range_t {
	static constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) {
		auto it = lower_bound(OPERATORS_FORWARD(sorted), value, cmp);
		return range_view(
			it,
			upper_bound(range_view(it, containers::end(OPERATORS_FORWARD(sorted))), value, cmp)
		);
	}
	static constexpr auto operator()(range auto && sorted, auto const & value) {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less());
	}
};
export constexpr auto equal_range = equal_range_t();


// TODO: This is sub-optimal. Unsure how to make it optimal without copy + paste
struct binary_search_t {
	static constexpr bool operator()(range auto && sorted, auto const & value, auto cmp) {
		auto const it = lower_bound(OPERATORS_FORWARD(sorted), value, cmp);
		return it != containers::end(sorted) and !cmp(value, *it);
	}
	static constexpr bool operator()(range auto && sorted, auto const & value) {
		return operator()(sorted, value, std::less());
	}
};
export constexpr auto binary_search = binary_search_t();

} // namespace containers

constexpr auto zero = containers::array<int, 0_bi>{};
constexpr auto one = containers::array{1};
constexpr auto two = containers::array{1, 2};
constexpr auto three = containers::array{1, 2, 3};
constexpr auto two_duplicates = containers::array{1, 1};
constexpr auto three_duplicates = containers::array{1, 1, 1};
constexpr auto three_two_duplicates_first = containers::array{1, 1, 2};
constexpr auto three_two_duplicates_last = containers::array{1, 2, 2};

static_assert(containers::lower_bound(zero, 0) == end(zero));
static_assert(containers::upper_bound(zero, 0) == end(zero));
static_assert(!containers::binary_search(zero, 0));

static_assert(containers::lower_bound(one, 0) == begin(one));
static_assert(containers::lower_bound(one, 1) == begin(one));
static_assert(containers::lower_bound(one, 2) == end(one));
static_assert(containers::upper_bound(one, 0) == begin(one));
static_assert(containers::upper_bound(one, 1) == end(one));
static_assert(containers::upper_bound(one, 2) == end(one));
static_assert(!containers::binary_search(one, 0));
static_assert(containers::binary_search(one, 1));
static_assert(!containers::binary_search(one, 2));

static_assert(containers::lower_bound(two, 0) == begin(two) + 0_bi);
static_assert(containers::lower_bound(two, 1) == begin(two) + 0_bi);
static_assert(containers::lower_bound(two, 2) == begin(two) + 1_bi);
static_assert(containers::lower_bound(two, 3) == end(two));
static_assert(containers::upper_bound(two, 0) == begin(two) + 0_bi);
static_assert(containers::upper_bound(two, 1) == begin(two) + 1_bi);
static_assert(containers::upper_bound(two, 2) == begin(two) + 2_bi);
static_assert(containers::upper_bound(two, 3) == end(two));
static_assert(!containers::binary_search(two, 0));
static_assert(containers::binary_search(two, 1));
static_assert(containers::binary_search(two, 2));
static_assert(!containers::binary_search(two, 3));

static_assert(containers::lower_bound(three, 0) == begin(three) + 0_bi);
static_assert(containers::lower_bound(three, 1) == begin(three) + 0_bi);
static_assert(containers::lower_bound(three, 2) == begin(three) + 1_bi);
static_assert(containers::lower_bound(three, 3) == begin(three) + 2_bi);
static_assert(containers::lower_bound(three, 4) == end(three));
static_assert(containers::upper_bound(three, 0) == begin(three) + 0_bi);
static_assert(containers::upper_bound(three, 1) == begin(three) + 1_bi);
static_assert(containers::upper_bound(three, 2) == begin(three) + 2_bi);
static_assert(containers::upper_bound(three, 3) == begin(three) + 3_bi);
static_assert(containers::upper_bound(three, 4) == end(three));
static_assert(!containers::binary_search(three, 0));
static_assert(containers::binary_search(three, 1));
static_assert(containers::binary_search(three, 2));
static_assert(containers::binary_search(three, 3));
static_assert(!containers::binary_search(three, 4));

static_assert(containers::lower_bound(two_duplicates, 0) == begin(two_duplicates) + 0_bi);
static_assert(containers::lower_bound(two_duplicates, 1) == begin(two_duplicates) + 0_bi);
static_assert(containers::lower_bound(two_duplicates, 2) == end(two_duplicates));
static_assert(containers::upper_bound(two_duplicates, 0) == begin(two_duplicates) + 0_bi);
static_assert(containers::upper_bound(two_duplicates, 1) == end(two_duplicates));
static_assert(containers::upper_bound(two_duplicates, 2) == end(two_duplicates));
static_assert(!containers::binary_search(two_duplicates, 0));
static_assert(containers::binary_search(two_duplicates, 1));
static_assert(!containers::binary_search(two_duplicates, 2));

static_assert(containers::lower_bound(three_duplicates, 0) == begin(three_duplicates) + 0_bi);
static_assert(containers::lower_bound(three_duplicates, 1) == begin(three_duplicates) + 0_bi);
static_assert(containers::lower_bound(three_duplicates, 2) == end(three_duplicates));
static_assert(containers::upper_bound(three_duplicates, 0) == begin(three_duplicates) + 0_bi);
static_assert(containers::upper_bound(three_duplicates, 1) == end(three_duplicates));
static_assert(containers::upper_bound(three_duplicates, 2) == end(three_duplicates));
static_assert(!containers::binary_search(three_duplicates, 0));
static_assert(containers::binary_search(three_duplicates, 1));
static_assert(!containers::binary_search(three_duplicates, 2));

static_assert(containers::lower_bound(three_two_duplicates_first, 0) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::lower_bound(three_two_duplicates_first, 1) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::lower_bound(three_two_duplicates_first, 2) == begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::lower_bound(three_two_duplicates_first, 3) == end(three_two_duplicates_first));
static_assert(containers::upper_bound(three_two_duplicates_first, 0) == begin(three_two_duplicates_first) + 0_bi);
static_assert(containers::upper_bound(three_two_duplicates_first, 1) == begin(three_two_duplicates_first) + 2_bi);
static_assert(containers::upper_bound(three_two_duplicates_first, 2) == begin(three_two_duplicates_first) + 3_bi);
static_assert(containers::upper_bound(three_two_duplicates_first, 3) == end(three_two_duplicates_first));
static_assert(!containers::binary_search(three_two_duplicates_first, 0));
static_assert(containers::binary_search(three_two_duplicates_first, 1));
static_assert(containers::binary_search(three_two_duplicates_first, 2));
static_assert(!containers::binary_search(three_two_duplicates_first, 3));

static_assert(containers::lower_bound(three_two_duplicates_last, 0) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::lower_bound(three_two_duplicates_last, 1) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::lower_bound(three_two_duplicates_last, 2) == begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::lower_bound(three_two_duplicates_last, 3) == end(three_two_duplicates_last));
static_assert(containers::upper_bound(three_two_duplicates_last, 0) == begin(three_two_duplicates_last) + 0_bi);
static_assert(containers::upper_bound(three_two_duplicates_last, 1) == begin(three_two_duplicates_last) + 1_bi);
static_assert(containers::upper_bound(three_two_duplicates_last, 2) == begin(three_two_duplicates_last) + 3_bi);
static_assert(containers::upper_bound(three_two_duplicates_last, 3) == end(three_two_duplicates_last));
static_assert(!containers::binary_search(three_two_duplicates_last, 0));
static_assert(containers::binary_search(three_two_duplicates_last, 1));
static_assert(containers::binary_search(three_two_duplicates_last, 2));
static_assert(!containers::binary_search(three_two_duplicates_last, 3));
