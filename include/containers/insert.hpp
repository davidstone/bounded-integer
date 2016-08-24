// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>
#include <containers/repeat_n.hpp>

#include <bounded/integer.hpp>

#include <initializer_list>
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
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	container.insert(position, begin(init), end(init))
)
// TODO: noexcept
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container> and std::numeric_limits<Size>::is_integer)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	return container.insert(position, begin(range), end(range));
}

}	// namespace common
}	// namespace detail
}	// namespace containers
