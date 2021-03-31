// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Size>
struct size_type_to_count_type {
	using type = Size;
};

template<bounded::bounded_integer Size>
struct size_type_to_count_type<Size> {
	using type  = bounded::integer<
		0,
		bounded::normalize<bounded::max_value<Size>.value()>
	>;
};

}

template<typename T>
using count_type = typename detail::size_type_to_count_type<typename std::decay_t<T>::size_type>::type;

}	// namespace containers
