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

template<typename Difference>
struct difference_type_to_offset_type {
	using type = Difference;
};

template<bounded::bounded_integer Difference>
struct difference_type_to_offset_type<Difference> {
	using type = bounded::integer<
		0,
		bounded::normalize<numeric_traits::max_value<Difference>>
	>;
};

} // namespace detail

template<typename T>
using offset_type = typename detail::difference_type_to_offset_type<iter_difference_t<T>>::type;

} // namespace containers
