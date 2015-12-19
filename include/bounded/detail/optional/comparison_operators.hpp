// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/optional/optional.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_builtin.hpp>

#include <utility>

namespace bounded {

template<typename LHS, typename RHS>
constexpr auto operator==(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(std::declval<LHS const &>() == std::declval<RHS const &>())) {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs == *rhs :
		(static_cast<bool>(lhs) == static_cast<bool>(rhs));
}


template<typename LHS, typename RHS>
constexpr auto operator<(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(std::declval<LHS const &>() < std::declval<RHS const &>())) {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs < *rhs :
		(static_cast<bool>(lhs) < static_cast<bool>(rhs));
}




template<typename LHS, typename RHS>
constexpr auto operator==(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(std::declval<LHS const &>() == rhs)) {
	return static_cast<bool>(lhs) and *lhs == rhs;
}

template<typename LHS, typename RHS>
constexpr auto operator==(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs == lhs)) {
	return rhs == lhs;
}



template<typename LHS, typename RHS>
constexpr auto operator<(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(std::declval<LHS const &>() == rhs)) {
	return !static_cast<bool>(lhs) ? true : *lhs < rhs;
}

template<typename LHS, typename RHS>
constexpr auto operator<(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(lhs < std::declval<RHS const &>())) {
	return !static_cast<bool>(rhs) ? false : lhs < *rhs;
}





template<typename T>
constexpr auto operator==(optional<T> const & lhs, none_t) noexcept {
	return !lhs;
}

template<typename T>
constexpr auto operator==(none_t, optional<T> const & rhs) noexcept {
	return !rhs;
}


template<typename T>
constexpr auto operator<(optional<T> const &, none_t) noexcept {
	return false;
}

template<typename T>
constexpr auto operator<(none_t, optional<T> const & rhs) noexcept {
	return static_cast<bool>(rhs);
}

}	// namespace bounded
