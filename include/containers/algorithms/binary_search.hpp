// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/partition.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/range_view.hpp>

#include <operators/forward.hpp>

#include <functional>

namespace containers {

struct lower_bound_t {
	static constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return cmp(element, value); });
	}
	static constexpr auto operator()(range auto && sorted, auto const & value) {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less());
	}
} constexpr inline lower_bound;

struct upper_bound_t {
	static constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return !cmp(value, element); });
	}
	static constexpr auto operator()(range auto && sorted, auto const & value) {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less());
	}
} constexpr inline upper_bound;

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
} constexpr inline equal_range;


// TODO: This is sub-optimal. Unsure how to make it optimal without copy + paste
struct binary_search_t {
	static constexpr bool operator()(range auto && sorted, auto const & value, auto cmp) {
		auto const it = lower_bound(OPERATORS_FORWARD(sorted), value, cmp);
		return it != containers::end(sorted) and !cmp(value, *it);
	}
	static constexpr bool operator()(range auto && sorted, auto const & value) {
		return operator()(sorted, value, std::less());
	}
} constexpr inline binary_search;

}	// namespace containers
