// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.index_type;

import containers.size;

import bounded;
import numeric_traits;

import std_module;

namespace containers {

template<typename Size>
struct to_index_type_impl {
	using type = Size;
};

template<bounded::bounded_integer Size>
struct to_index_type_impl<Size> {
	using type = bounded::integer<
		0,
		bounded::normalize<numeric_traits::max_value<Size> - bounded::constant<1>>
	>;
};

export template<typename Size>
using to_index_type = typename to_index_type_impl<Size>::type;

export template<typename T>
using index_type = to_index_type<range_size_t<T>>;

} // namespace containers
