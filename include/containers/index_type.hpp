// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <limits>
#include <type_traits>

namespace containers {
namespace detail {

template<typename T>
constexpr auto index_type_source() -> typename T::size_type {
	return std::numeric_limits<typename T::size_type>::min();
}
template<typename T>
constexpr auto index_type_source() -> typename T::difference_type {
	return std::numeric_limits<typename T::difference_type>::min();
}

}	// namespace detail

// TODO: ???
template<typename T>
using index_type = bounded::checked_integer<
	0,
	static_cast<std::intmax_t>(std::numeric_limits<decltype(detail::index_type_source<std::decay_t<T>>())>::max()) - 1,
	std::out_of_range
>;

}	// namespace containers
