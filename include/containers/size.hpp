// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/distance.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

// Ignored... makes this a worse match than std::size if that compiles
template<typename Range, typename... Ignored> requires(
	is_range<Range> and
	bounded::is_bounded_integer<typename Range::size_type>
)
constexpr auto size(Range const & range, Ignored && ...) noexcept {
	return typename Range::size_type(end(range) - begin(range), bounded::non_check);
}
template<typename Range, typename... Ignored> requires(
	is_range<Range> and
	!bounded::is_bounded_integer<typename Range::size_type>
)
constexpr auto size(Range const & range, Ignored && ...) noexcept {
	return typename Range::size_type(end(range) - begin(range));
}

}	// namespace common

using ::containers::detail::common::size;

template<typename, typename = void>
constexpr auto is_sized = false;

template<typename Range>
constexpr auto is_sized<
	Range,
	std::void_t<decltype(size(std::declval<Range const &>()))>
> = true;


template<typename Range>
constexpr auto linear_size(Range const & range) {
	if constexpr (is_sized<Range>) {
		return size(range);
	} else {
		return containers::distance(begin(range), end(range));
	}
}

}	// namespace detail

using ::containers::detail::common::size;

}	// namespace containers
