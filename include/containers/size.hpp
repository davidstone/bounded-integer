// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container> and bounded::is_bounded_integer<typename Container::size_type>)>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(end(container) - begin(container), bounded::non_check);
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container> and !bounded::is_bounded_integer<typename Container::size_type>)>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(end(container) - begin(container));
}

}	// namespace common

using ::containers::detail::common::size;

}	// namespace detail

using ::containers::detail::common::size;

}	// namespace containers
