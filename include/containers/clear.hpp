// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename, typename = void>
constexpr auto has_pop_back = false;

template<typename Container>
constexpr auto has_pop_back<
	Container,
	std::void_t<decltype(std::declval<Container &>().pop_back())>
> = true;

template<typename, typename = void>
constexpr auto has_pop_front = false;

template<typename Container>
constexpr auto has_pop_front<
	Container,
	std::void_t<decltype(std::declval<Container &>().pop_front())>
> = true;

namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto clear(Container & container) noexcept {
	if constexpr (has_pop_back<Container>) {
		while (!empty(container)) {
			container.pop_back();
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
