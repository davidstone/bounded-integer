// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>
#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[typename index_type<Container>::overflow_policy{}.assignment(
		bounded::make(index),
		0_bi,
		size(container) - 1_bi
	)]
)

template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index, bounded::non_check_t) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[static_cast<index_type<Container>>(index)]
)


}	// namespace common
}	// namespace detail
}	// namespace containers
