// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Iterator>
concept has_reference_type = requires { typename Iterator::reference_type; };

template<typename Iterator, typename T>
concept reference_type_matches =
	!has_reference_type<Iterator> or
	std::same_as<typename Iterator::reference_type, T>;

template<typename Iterator>
struct iter_reference_t_impl {
	using type = decltype(*std::declval<Iterator>());
	static_assert(reference_type_matches<Iterator, type>);
};

} // namespace detail

template<iterator Iterator>
using iter_reference_t = typename detail::iter_reference_t_impl<Iterator>::type;

} // namespace containers
