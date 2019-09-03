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

template<typename Range>
concept has_member_size = requires(Range const & range) { range.size(); };

template<typename Range>
concept random_access_range = requires(Range const & range) {
	static_cast<typename Range::size_type>(end(range) - begin(range));
};

namespace common {

template<typename Range> requires has_member_size<Range> or random_access_range<Range>
constexpr auto size(Range const & range) noexcept {
	if constexpr (has_member_size<Range>) {
		return range.size();
	} else if constexpr (bounded::is_bounded_integer<typename Range::size_type>) {
		return typename Range::size_type(end(range) - begin(range), bounded::non_check);
	} else {
		return typename Range::size_type(end(range) - begin(range));
	}
}

}	// namespace common

using ::containers::detail::common::size;

template<typename, typename = void>
inline constexpr auto is_sized = false;

template<typename Range>
inline constexpr auto is_sized<
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
