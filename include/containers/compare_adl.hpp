// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace compare_adl {

using bounded::compare;

template<typename LHS, typename RHS>
constexpr auto indirect_compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs)
)

}	// namespace compare_adl
}	// namespace detail
}	// namespace containers
