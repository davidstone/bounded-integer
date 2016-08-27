// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace_back(value)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace_back(std::move(value))
)

}	// namespace common

using ::containers::detail::common::push_back;

}	// namespace detail

using ::containers::detail::common::push_back;

}	// namespace containers
