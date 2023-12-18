// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iter_value_t;

import containers.common_iterator_functions;
import containers.iterator;

import bounded;
import std_module;

namespace containers {

template<typename Iterator>
concept has_value_type = requires { typename Iterator::value_type; };

template<typename Iterator, typename T>
concept value_type_matches =
	!has_value_type<Iterator> or
	std::same_as<typename Iterator::value_type, T>;

template<typename Iterator>
struct iter_value_t_impl {
	using type = std::remove_cvref_t<decltype(*bounded::declval<Iterator>())>;
	static_assert(value_type_matches<Iterator, type>);
};

export template<iterator Iterator>
using iter_value_t = typename iter_value_t_impl<Iterator>::type;

} // namespace containers