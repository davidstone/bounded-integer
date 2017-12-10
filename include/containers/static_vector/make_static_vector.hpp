// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/static_vector/static_vector.hpp>

#include <limits>
#include <type_traits>

namespace containers {

template<typename T, typename Size>
constexpr auto make_static_vector(Size const size) BOUNDED_NOEXCEPT(
	static_vector<T, static_cast<std::intmax_t>(std::numeric_limits<Size>::max())>(size)
)
template<typename T, typename Size>
constexpr auto make_static_vector(Size const size, T const & value) BOUNDED_NOEXCEPT(
	static_vector<std::decay_t<T>, static_cast<std::intmax_t>(std::numeric_limits<Size>::max())>(size, value)
)

}	// namespace containers
