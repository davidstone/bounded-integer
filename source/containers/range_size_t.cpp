// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.range_size_t;

import containers.has_member_size;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.range;

import bounded;
import std_module;

namespace containers {

template<range T>
struct size_type_impl {
	using type = bounded::integer<
		0,
		bounded::builtin_max_value<iter_difference_t<iterator_t<T>>>
	>;
};

template<typename T>
concept has_size_type = requires { typename T::size_type; };

template<range T> requires has_size_type<T> and has_member_size<T>
struct size_type_impl<T> {
	static_assert(std::same_as<decltype(bounded::declval<T const &>().size()), typename T::size_type>);
	using type = typename T::size_type;
};

template<range T> requires has_size_type<T>
struct size_type_impl<T> {
	using type = typename T::size_type;
};

template<range T> requires has_member_size<T>
struct size_type_impl<T> {
	using type = decltype(bounded::declval<T const &>().size());
};

export template<typename T>
using range_size_t = typename size_type_impl<std::remove_cvref_t<T>>::type;

} // namespace containers
