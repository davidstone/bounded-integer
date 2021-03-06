// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Iterator>
struct iter_value_t_impl {
	using type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;
};

template<typename T>
concept has_value_type = requires { typename T::value_type; };

template<has_value_type Iterator>
struct iter_value_t_impl<Iterator> {
	using type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;
	static_assert(std::is_same_v<type, typename Iterator::value_type>);
};

} // namespace detail

template<iterator Iterator>
using iter_value_t = typename detail::iter_value_t_impl<Iterator>::type;

} // namespace containers
