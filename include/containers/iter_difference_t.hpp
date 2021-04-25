// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace containers {

namespace detail {

template<typename Iterator>
struct iter_difference_t_impl {
	using type = typename Iterator::difference_type;
};

template<typename Iterator>
concept subtractable = requires(Iterator it) { it - it; };

template<subtractable Iterator>
struct iter_difference_t_impl<Iterator> {
	using type = decltype(std::declval<Iterator>() - std::declval<Iterator>());
private:
	static void check() {
		if constexpr (requires { typename Iterator::difference_type; }) {
			static_assert(std::is_same_v<typename Iterator::difference_type, type>);
		}
	}
};

} // namespace detail

template<typename Iterator>
using iter_difference_t = typename detail::iter_difference_t_impl<std::remove_reference_t<Iterator>>::type;

} // namespace containers
