// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.binary_search;

import containers.algorithms.partition;
import containers.begin_end;
import containers.range;
import containers.subrange;

import std_module;

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
		return subrange(
			it,
			upper_bound(subrange(it, containers::end(OPERATORS_FORWARD(sorted))), value, cmp)
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
