// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "compound_assignment.hpp"
#include "../is_bounded_integer.hpp"
#include "../noexcept.hpp"

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(symbol) \
template<typename LHS, typename RHS, BOUNDED_REQUIRES(!is_bounded_integer<LHS>)> \
constexpr auto operator symbol##=(LHS & lhs, RHS && rhs) BOUNDED_NOEXCEPT_DECLTYPE( \
	lhs = std::move(lhs) symbol std::forward<RHS>(rhs) \
) \

BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(+)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(-)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(*)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(/)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(%)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(<<)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(>>)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(&)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(|)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(^)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR


template<typename T>
constexpr auto operator++(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value += constant<1>
)

template<typename T>
constexpr auto operator++(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>::value and std::is_nothrow_move_constructible<T>::value and noexcept(++value)) {
	auto previous = value;
	++value;
	return previous;
}


template<typename T>
constexpr auto operator--(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value -= constant<1>
)

template<typename T>
constexpr auto operator--(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>::value and std::is_nothrow_move_constructible<T>::value and noexcept(--value)) {
	auto previous = value;
	--value;
	return previous;
}

#define BOUNDED_COMMON_ARITHMETIC \
	using bounded::detail::arithmetic::operator+=; \
	using bounded::detail::arithmetic::operator-=; \
	using bounded::detail::arithmetic::operator*=; \
	using bounded::detail::arithmetic::operator/=; \
	using bounded::detail::arithmetic::operator%=; \
	using bounded::detail::arithmetic::operator<<=; \
	using bounded::detail::arithmetic::operator>>=; \
	using bounded::detail::arithmetic::operator&=; \
	using bounded::detail::arithmetic::operator|=; \
	using bounded::detail::arithmetic::operator^=; \
	using bounded::detail::arithmetic::operator++; \
	using bounded::detail::arithmetic::operator--; \
	using bounded::detail::arithmetic::operator-;

}	// namespace arithmetic

BOUNDED_COMMON_ARITHMETIC

}	// namespace detail

BOUNDED_COMMON_ARITHMETIC

}	// namespace bounded
