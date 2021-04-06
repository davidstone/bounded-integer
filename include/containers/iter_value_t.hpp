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

template<typename Iterator> requires requires { typename Iterator::value_type; }
struct iter_value_t_impl<Iterator> {
	using specified_type = typename Iterator::value_type;
	using type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;
	static_assert(std::is_same_v<specified_type, type>);
};

} // namespace detail

template<iterator Iterator>
using iter_value_t = typename detail::iter_value_t_impl<Iterator>::type;

} // namespace containers
