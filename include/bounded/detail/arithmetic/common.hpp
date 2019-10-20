// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/returns.hpp>

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(symbol) \
constexpr auto operator symbol##=(auto & lhs, auto && rhs) BOUNDED_RETURNS( \
	lhs = std::move(lhs) symbol BOUNDED_FORWARD(rhs) \
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

constexpr auto operator++(auto & value) BOUNDED_RETURNS(
	value += constant<1>
)

template<typename T>
concept incrementable = requires(T value) { ++value; };

template<typename T>
concept generic_postfix_incrementable = incrementable<T> and std::is_copy_constructible_v<T>;

constexpr auto operator++(generic_postfix_incrementable auto & value, int) {
	auto previous = value;
	++value;
	return previous;
}


constexpr auto operator--(auto & value) BOUNDED_RETURNS(
	value -= constant<1>
)

template<typename T>
concept decrementable = requires(T value) { --value; };

template<typename T>
concept generic_postfix_decrementable = decrementable<T> and std::is_copy_constructible_v<T>;

constexpr auto operator--(generic_postfix_decrementable auto & value, int) {
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
