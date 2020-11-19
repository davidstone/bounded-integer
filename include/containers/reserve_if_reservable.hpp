// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/size.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
concept reservable = requires(Container & container, typename Container::size_type size) { container.reserve(size); };

template<typename Container>
constexpr void reserve_if_reservable(Container & container, typename Container::size_type const size) {
	if constexpr (reservable<Container>) {
		container.reserve(size);
	}
}

template<typename Container>
constexpr auto reallocation_size(Container const & container, auto const count) {
	return static_cast<typename Container::size_type>(bounded::max(
		containers::size(container) + count,
		container.capacity() * 2_bi
	));
}

template<typename Container>
constexpr void growth_reallocation(Container & container, auto const count) {
	if constexpr (reservable<Container>) {
		container.reserve(::containers::detail::reallocation_size(container, count));
	} else {
		BOUNDED_ASSERT(container.capacity() >= size(container) + count);
	}
}

}	// namespace detail
}	// namespace containers
