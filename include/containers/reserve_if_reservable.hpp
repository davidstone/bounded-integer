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

template<typename Capacity>
constexpr auto reallocation_size(Capacity const current_capacity, auto const current_size, auto const extra_elements) {
	return static_cast<Capacity>(bounded::max(
		current_size + extra_elements,
		current_capacity * 2_bi
	));
}

template<typename Range>
concept size_then_use_range = forward_range<Range const &> or requires(Range const & r) { containers::size(r); };

} // namespace detail
} // namespace containers
