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

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto data(Container && container) BOUNDED_NOEXCEPT_DECLTYPE(
	pointer_from(begin(container))
)

}	// namespace common
}	// namespace detail
}	// namespace containers
