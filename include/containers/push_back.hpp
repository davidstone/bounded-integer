// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
namespace common {

template<typename, typename = void>
constexpr auto has_member_push_back = false;

template<typename Container>
constexpr auto has_member_push_back<
	Container,
	std::void_t<decltype(std::declval<Container &>().push_back(std::declval<typename Container::value_type>()))>
> = true;

template<typename Container> requires(!has_member_push_back<Container>)
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT_DECLTYPE(
	container.emplace_back(value)
)
template<typename Container> requires(!has_member_push_back<Container>)
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT_DECLTYPE(
	container.emplace_back(std::move(value))
)

template<typename Container> requires has_member_push_back<Container>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT_DECLTYPE(
	container.push_back(value)
)
template<typename Container> requires has_member_push_back<Container>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT_DECLTYPE(
	container.push_back(std::move(value))
)

}	// namespace common

using ::containers::detail::common::push_back;

}	// namespace detail

using ::containers::detail::common::push_back;

}	// namespace containers
