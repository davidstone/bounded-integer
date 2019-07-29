// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <functional>
#include <limits>

namespace containers {

constexpr inline struct partition_point_t {
	template<typename Range, typename UnaryPredicate> requires is_range<Range>
	constexpr auto operator()(Range && range, UnaryPredicate predicate) const {
		using size_type = decltype(size(range));
		auto count = bounded::integer<0, bounded::detail::normalize<std::numeric_limits<size_type>::max().value()>>(size(range));
		auto first = begin(range);
		if constexpr (count.max() == bounded::constant<0>) {
			return first;
		} else {
			while (count > bounded::constant<0>) {
				auto it = first;
				auto const step = count / bounded::constant<2>;
				::containers::advance(it, step);
				if (predicate(*it)) {
					first = ::containers::next(it);
					count -= step + bounded::constant<1>;
				} else {
					count = step;
				}
			}
			return first;
		}
	}
} partition_point;

constexpr inline struct lower_bound_t {
	template<typename Range, typename T, typename Compare> requires is_range<Range>
	constexpr auto operator()(Range && range, T const & value, Compare cmp) const {
		return partition_point(BOUNDED_FORWARD(range), [&](auto const & element) { return cmp(element, value); });
	}
	template<typename Range, typename T> requires is_range<Range>
	constexpr auto operator()(Range && range, T const & value) const {
		return operator()(BOUNDED_FORWARD(range), value, std::less{});
	}
} lower_bound;

constexpr inline struct upper_bound_t {
	template<typename Range, typename T, typename Compare> requires is_range<Range>
	constexpr auto operator()(Range && range, T const & value, Compare cmp) const {
		return partition_point(BOUNDED_FORWARD(range), [&](auto const & element) { return !cmp(value, element); });
	}
	template<typename Range, typename T> requires is_range<Range>
	constexpr auto operator()(Range && range, T const & value) const {
		return operator()(BOUNDED_FORWARD(range), value, std::less{});
	}
} upper_bound;


// TODO: This is sub-optimal. Unsure how to make it optimal without copy + paste
constexpr inline struct binary_search_t {
	template<typename Range, typename T, typename Compare> requires is_range<Range>
	constexpr bool operator()(Range && range, T const & value, Compare cmp) const {
		return lower_bound(BOUNDED_FORWARD(range), value, cmp);
	}
	template<typename Range, typename T> requires is_range<Range>
	constexpr bool operator()(Range && range, T const & value) const {
		return operator()(range, value, std::less{});
	}
} binary_search;

}	// namespace containers
