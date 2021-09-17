// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace containers {
namespace detail {

template<typename T>
struct iter_difference_t_impl;

template<typename T>
concept has_difference_type = requires { typename T::difference_type; };

template<typename Iterator>
concept subtractable = requires(Iterator const it) { it - it; };

template<typename T> requires has_difference_type<T> and subtractable<T>
struct iter_difference_t_impl<T> {
	static_assert(std::is_same_v<decltype(std::declval<T const &>() - std::declval<T const &>()), typename T::difference_type>);
	using type = typename T::difference_type;
};

template<has_difference_type T>
struct iter_difference_t_impl<T> {
	using type = typename T::difference_type;
};

template<subtractable T>
struct iter_difference_t_impl<T> {
	using type = decltype(std::declval<T const &>() - std::declval<T const &>());
};

} // namespace detail

template<typename Iterator> requires
	detail::has_difference_type<std::remove_reference_t<Iterator>> or
	detail::subtractable<std::remove_reference_t<Iterator>>
using iter_difference_t = typename detail::iter_difference_t_impl<std::remove_reference_t<Iterator>>::type;

} // namespace containers
