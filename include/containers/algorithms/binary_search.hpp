// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/partition.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>

#include <functional>

namespace containers {

constexpr inline struct lower_bound_t {
	template<typename T, typename Compare>
	constexpr auto operator()(range auto && sorted, T const & value, Compare cmp) const {
		return partition_point(BOUNDED_FORWARD(sorted), [&](auto const & element) { return cmp(element, value); });
	}
	template<typename T>
	constexpr auto operator()(range auto && sorted, T const & value) const {
		return operator()(BOUNDED_FORWARD(sorted), value, std::less{});
	}
} lower_bound;

constexpr inline struct upper_bound_t {
	template<typename T, typename Compare>
	constexpr auto operator()(range auto && sorted, T const & value, Compare cmp) const {
		return partition_point(BOUNDED_FORWARD(sorted), [&](auto const & element) { return !cmp(value, element); });
	}
	template<typename T>
	constexpr auto operator()(range auto && sorted, T const & value) const {
		return operator()(BOUNDED_FORWARD(sorted), value, std::less{});
	}
} upper_bound;


// TODO: This is sub-optimal. Unsure how to make it optimal without copy + paste
constexpr inline struct binary_search_t {
	template<typename T, typename Compare>
	constexpr bool operator()(range auto && sorted, T const & value, Compare cmp) const {
		return lower_bound(BOUNDED_FORWARD(sorted), value, cmp) == end(sorted);
	}
	template<typename T>
	constexpr bool operator()(range auto && sorted, T const & value) const {
		return operator()(sorted, value, std::less{});
	}
} binary_search;

}	// namespace containers
