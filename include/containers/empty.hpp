// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Container>
constexpr auto never_empty = std::numeric_limits<typename std::remove_reference_t<Container>::size_type>::min() > 0_bi;

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto empty(Container const & container) noexcept {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty<Container> ? false : begin(container) == end(container);
}

}	// namespace common
}	// namespace detail
}	// namespace containers
