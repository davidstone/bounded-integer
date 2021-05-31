// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <limits>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Size>
struct size_type_to_index_type {
	using type = Size;
};

template<bounded::bounded_integer Size>
struct size_type_to_index_type<Size> {
	using type  = bounded::integer<
		0,
		bounded::normalize<numeric_traits::max_value<Size> - bounded::constant<1>>
	>;
};

}

template<typename T>
using index_type = typename detail::size_type_to_index_type<range_size_t<T>>::type;

}	// namespace containers
