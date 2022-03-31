// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iter_difference_t.hpp>

#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace containers {
namespace detail {

template<typename T>
using to_offset_type = std::conditional_t<
	bounded::bounded_integer<T>,
	bounded::integer<0, bounded::builtin_max_value<T>>,
	T
>;

} // namespace detail

template<typename T>
using offset_type = detail::to_offset_type<iter_difference_t<T>>;

} // namespace containers
