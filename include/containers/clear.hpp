// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto clear(Container & container) noexcept {
	container = Container{};
}

}	// namespace common

using ::containers::detail::common::clear;

}	// namespace detail

using ::containers::detail::common::clear;

}	// namespace containers
