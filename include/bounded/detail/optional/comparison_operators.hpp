// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/optional/optional.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_builtin.hpp>
#include <bounded/detail/noexcept.hpp>

#include <utility>

namespace bounded {

template<typename LHS, typename RHS>
constexpr auto compare(optional<LHS> const & lhs, optional<RHS> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	(static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		compare(*lhs, *rhs) :
		::bounded::compare(static_cast<bool>(lhs), static_cast<bool>(rhs))
)


template<typename LHS, typename RHS>
constexpr auto compare(optional<LHS> const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_VALUE(
	static_cast<bool>(lhs) ? compare(*lhs, rhs) : strong_ordering_less
)

template<typename LHS, typename RHS>
constexpr auto compare(LHS const & lhs, optional<RHS> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	static_cast<bool>(rhs) ? compare(lhs, *rhs) : strong_ordering_greater
)


template<typename T>
constexpr auto compare(optional<T> const & lhs, none_t) noexcept {
	return static_cast<bool>(lhs) ? strong_ordering_greater : strong_ordering_equal;
}

template<typename T>
constexpr auto compare(none_t, optional<T> const & rhs) noexcept {
	return static_cast<bool>(rhs) ? strong_ordering_less : strong_ordering_equal;
}

}	// namespace bounded
