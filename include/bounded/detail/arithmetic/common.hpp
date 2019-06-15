// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/noexcept.hpp>

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

// We have to use static_cast here to avoid integral promotion warnings
#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(symbol) \
template<typename LHS, typename RHS> \
constexpr auto operator symbol##=(LHS & lhs, RHS && rhs) BOUNDED_NOEXCEPT_DECLTYPE( \
	lhs = static_cast<LHS>(std::move(lhs) symbol BOUNDED_FORWARD(rhs)) \
)

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

template<typename T, typename Enable = void>
constexpr auto incrementable = false;

template<typename T>
constexpr auto incrementable<T, std::void_t<decltype(++std::declval<T &>())>> = true;

template<typename T> requires(incrementable<T> and std::is_copy_constructible_v<T>)
constexpr auto operator++(T & value, int) noexcept(std::is_nothrow_copy_constructible_v<T> and std::is_nothrow_move_constructible_v<T> and noexcept(++value)) {
	auto previous = value;
	++value;
	return previous;
}


template<typename T>
constexpr auto operator--(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value -= constant<1>
)

template<typename T, typename Enable = void>
constexpr auto decrementable = false;

template<typename T>
constexpr auto decrementable<T, std::void_t<decltype(--std::declval<T &>())>> = true;

template<typename T> requires(decrementable<T>)
constexpr auto operator--(T & value, int) noexcept(std::is_nothrow_copy_constructible_v<T> and std::is_nothrow_move_constructible_v<T> and noexcept(--value)) {
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
