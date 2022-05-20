// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Range>
concept never_empty_range = numeric_traits::min_value<range_size_t<Range>> > bounded::constant<0>;

} // namespace detail

constexpr auto is_empty = [](range auto && r) {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return detail::never_empty_range<decltype(r)> ? false : containers::begin(r) == containers::end(r);
};

} // namespace containers
