// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/partition.hpp>
#include <containers/is_range.hpp>

#include <operators/forward.hpp>

#include <functional>

namespace containers {

constexpr inline struct lower_bound_t {
	constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) const {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return cmp(element, value); });
	}
	constexpr auto operator()(range auto && sorted, auto const & value) const {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less{});
	}
} lower_bound;

constexpr inline struct upper_bound_t {
	constexpr auto operator()(range auto && sorted, auto const & value, auto cmp) const {
		return partition_point(OPERATORS_FORWARD(sorted), [&](auto const & element) { return !cmp(value, element); });
	}
	constexpr auto operator()(range auto && sorted, auto const & value) const {
		return operator()(OPERATORS_FORWARD(sorted), value, std::less{});
	}
} upper_bound;


// TODO: This is sub-optimal. Unsure how to make it optimal without copy + paste
constexpr inline struct binary_search_t {
	constexpr bool operator()(range auto && sorted, auto const & value, auto cmp) const {
		return lower_bound(OPERATORS_FORWARD(sorted), value, cmp) == end(sorted);
	}
	constexpr bool operator()(range auto && sorted, auto const & value) const {
		return operator()(sorted, value, std::less{});
	}
} binary_search;

}	// namespace containers
