// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/pop_back.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename, typename = void>
inline constexpr auto has_pop_front = false;

template<typename Container>
inline constexpr auto has_pop_front<
	Container,
	std::void_t<decltype(std::declval<Container &>().pop_front())>
> = true;

namespace common {

template<typename Container> requires is_container<Container>
constexpr auto clear(Container & container) noexcept {
	if constexpr (pop_backable<Container>) {
		while (!empty(container)) {
			pop_back(container);
		}
	} else if constexpr (has_pop_front<Container>) {
		while (!empty(container)) {
			container.pop_front();
		}
	} else {
		container = {};
	}
}

}	// namespace common

using ::containers::detail::common::clear;

}	// namespace detail

using ::containers::detail::common::clear;

}	// namespace containers
