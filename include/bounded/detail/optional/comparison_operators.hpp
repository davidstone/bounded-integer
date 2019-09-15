// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/optional/optional.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_mixed.hpp>

#include <utility>

namespace bounded {

template<typename LHS, typename RHS> requires equality_comparable<LHS, RHS>
constexpr auto operator==(optional<LHS> const & lhs, optional<RHS> const & rhs) {
	return (lhs and rhs) ?
		*lhs == *rhs :
		static_cast<bool>(lhs) == static_cast<bool>(rhs);
}


template<equality_comparable T>
constexpr auto operator==(optional<T> const & lhs, T const & rhs) {
	return static_cast<bool>(lhs) and *lhs == rhs;
}

template<equality_comparable T>
constexpr auto operator==(T const & lhs, optional<T> const & rhs) {
	return rhs == lhs;
}

template<typename T>
constexpr auto operator==(optional<T> const & lhs, none_t) {
	return !lhs;
}

template<typename T>
constexpr auto operator==(none_t, optional<T> const & rhs) {
	return !rhs;
}

}	// namespace bounded
