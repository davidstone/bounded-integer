// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/size.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename, typename = void>
constexpr auto has_reserve = false;

template<typename Container>
constexpr auto has_reserve<
	Container,
	std::void_t<decltype(std::declval<Container>().reserve(std::declval<typename Container::size_type>()))>
> = true;

template<typename Container, BOUNDED_REQUIRES(is_container<Container> and !has_reserve<Container>)>
constexpr void reserve_if_reservable(Container &, typename Container::size_type) noexcept {
}

template<typename Container, BOUNDED_REQUIRES(is_container<Container> and has_reserve<Container>)>
constexpr auto reserve_if_reservable(Container & container, typename Container::size_type const size) BOUNDED_NOEXCEPT_DECLTYPE(
	container.reserve(size)
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto reallocation_size(Container const & container, typename Container::size_type const count) BOUNDED_NOEXCEPT_VALUE(
	static_cast<typename Container::size_type>(bounded::max(size(container) + count, container.capacity() * 2_bi))
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container> and !has_reserve<Container>)>
constexpr void growth_reallocation(Container const & container, typename Container::size_type const count) noexcept {
	BOUNDED_ASSERT(container.capacity() >= size(container) + count);
}

template<typename Container, BOUNDED_REQUIRES(is_container<Container> and has_reserve<Container>)>
constexpr auto growth_reallocation(Container & container, typename Container::size_type const count) BOUNDED_NOEXCEPT_DECLTYPE(
	container.reserve(::containers::detail::reallocation_size(container, count))
)

}	// namespace detail
}	// namespace containers
