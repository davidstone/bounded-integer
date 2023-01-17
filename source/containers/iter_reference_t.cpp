// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iter_reference_t;

import containers.is_iterator;

import bounded;
import std_module;

namespace containers {

template<typename Iterator>
concept has_reference_type = requires { typename Iterator::reference_type; };

template<typename Iterator, typename T>
concept reference_type_matches =
	!has_reference_type<Iterator> or
	std::same_as<typename Iterator::reference_type, T>;

template<typename Iterator>
struct iter_reference_t_impl {
	using type = decltype(*bounded::declval<Iterator>());
	static_assert(reference_type_matches<Iterator, type>);
};

export template<iterator Iterator>
using iter_reference_t = typename iter_reference_t_impl<Iterator>::type;

} // namespace containers
