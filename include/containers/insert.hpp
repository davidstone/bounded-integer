// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>
#include <containers/repeat_n.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace(position, value)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace(position, std::move(value))
)

}	// namespace common

using ::containers::detail::common::insert;

}	// namespace detail

using ::containers::detail::common::insert;

}	// namespace containers
