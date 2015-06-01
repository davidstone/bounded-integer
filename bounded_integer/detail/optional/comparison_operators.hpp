// Optional comparison operators
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "optional.hpp"
#include "../class.hpp"
#include "../operators/comparison.hpp"
#include "../operators/comparison_builtin.hpp"
#include <utility>

namespace bounded {

template<typename LHS, typename RHS>
constexpr auto operator==(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(std::declval<LHS const &>() == std::declval<RHS const &>())) {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs == *rhs :
		(static_cast<bool>(lhs) == static_cast<bool>(rhs));
}

template<typename LHS, typename RHS>
constexpr auto operator!=(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(lhs == rhs)) {
	return !(lhs == rhs);
}

template<typename LHS, typename RHS>
constexpr auto operator<(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(std::declval<LHS const &>() < std::declval<RHS const &>())) {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs < *rhs :
		(static_cast<bool>(lhs) < static_cast<bool>(rhs));
}

template<typename LHS, typename RHS>
constexpr auto operator>(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs < lhs)) {
	return rhs < lhs;
}

template<typename LHS, typename RHS>
constexpr auto operator<=(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs < lhs)) {
	return !(rhs < lhs);
}

template<typename LHS, typename RHS>
constexpr auto operator>=(optional<LHS> const & lhs, optional<RHS> const & rhs) noexcept(noexcept(lhs < rhs)) {
	return !(lhs < rhs);
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
constexpr auto operator!=(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(lhs == rhs)) {
	return !(lhs == rhs);
}

template<typename LHS, typename RHS>
constexpr auto operator!=(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs == lhs)) {
	return !(rhs == lhs);
}



template<typename LHS, typename RHS>
constexpr auto operator<(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(std::declval<LHS const &>() == rhs)) {
	return !static_cast<bool>(lhs) ? true : *lhs < rhs;
}

template<typename LHS, typename RHS>
constexpr auto operator<(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(lhs < std::declval<RHS const &>())) {
	return !static_cast<bool>(rhs) ? false : lhs < *rhs;
}


template<typename LHS, typename RHS>
constexpr auto operator>(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(rhs < lhs)) {
	return rhs < lhs;
}

template<typename LHS, typename RHS>
constexpr auto operator>(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs < lhs)) {
	return rhs < lhs;
}


template<typename LHS, typename RHS>
constexpr auto operator<=(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(rhs < lhs)) {
	return !(rhs < lhs);
}

template<typename LHS, typename RHS>
constexpr auto operator<=(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(rhs < lhs)) {
	return !(rhs < lhs);
}


template<typename LHS, typename RHS>
constexpr auto operator>=(optional<LHS> const & lhs, RHS const & rhs) noexcept(noexcept(lhs < rhs)) {
	return !(lhs < rhs);
}

template<typename LHS, typename RHS>
constexpr auto operator>=(LHS const & lhs, optional<RHS> const & rhs) noexcept(noexcept(lhs < rhs)) {
	return !(lhs < rhs);
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
constexpr auto operator!=(optional<T> const & lhs, none_t const rhs) noexcept {
	return !(lhs == rhs);
}

template<typename T>
constexpr auto operator!=(none_t const lhs, optional<T> const & rhs) noexcept {
	return !(rhs == lhs);
}


template<typename T>
constexpr auto operator<(optional<T> const &, none_t) noexcept {
	return false;
}

template<typename T>
constexpr auto operator<(none_t, optional<T> const & rhs) noexcept {
	return static_cast<bool>(rhs);
}


template<typename T>
constexpr auto operator>(optional<T> const & lhs, none_t const rhs) noexcept {
	return rhs < lhs;
}

template<typename T>
constexpr auto operator>(none_t const lhs, optional<T> const & rhs) noexcept {
	return rhs < lhs;
}


template<typename T>
constexpr auto operator<=(optional<T> const & lhs, none_t const rhs) noexcept {
	return !(rhs < lhs);
}

template<typename T>
constexpr auto operator<=(none_t const lhs, optional<T> const & rhs) noexcept {
	return !(rhs < lhs);
}


template<typename T>
constexpr auto operator>=(optional<T> const & lhs, none_t const rhs) noexcept {
	return !(lhs < rhs);
}

template<typename T>
constexpr auto operator>=(none_t const lhs, optional<T> const & rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace bounded
