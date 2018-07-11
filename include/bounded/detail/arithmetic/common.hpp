// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/requires.hpp>

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR(symbol) \
template<typename LHS, typename RHS> \
constexpr auto operator symbol##=(LHS & lhs, RHS && rhs) BOUNDED_NOEXCEPT_DECLTYPE( \
	lhs = std::move(lhs) symbol std::forward<RHS>(rhs) \
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

template<typename...>
using void_t = void;

template<typename T, typename Enable = void>
struct incrementable_by_bounded_c : std::false_type {};

template<typename T>
struct incrementable_by_bounded_c<T, void_t<decltype(std::declval<T &>() += bounded::constant<1>)>> : std::true_type {};

template<typename T>
constexpr auto incrementable_by_bounded = incrementable_by_bounded_c<T>{};

template<typename T, BOUNDED_REQUIRES(incrementable_by_bounded<T>)>
constexpr auto operator++(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value += constant<1>
)
template<typename T, BOUNDED_REQUIRES(!incrementable_by_bounded<T>)>
constexpr auto operator++(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value += 1
)

template<typename T>
constexpr auto operator++(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>{} and std::is_nothrow_move_constructible<T>{} and noexcept(++value)) {
	auto previous = value;
	++value;
	return previous;
}


template<typename T, typename Enable = void>
struct decrementable_by_bounded_c : std::false_type {};

template<typename T>
struct decrementable_by_bounded_c<T, void_t<decltype(std::declval<T &>() -= bounded::constant<1>)>> : std::true_type {};

template<typename T>
constexpr auto decrementable_by_bounded = decrementable_by_bounded_c<T>{};

template<typename T, BOUNDED_REQUIRES(decrementable_by_bounded<T>)>
constexpr auto operator--(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value -= constant<1>
)
template<typename T, BOUNDED_REQUIRES(!decrementable_by_bounded<T>)>
constexpr auto operator--(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value -= 1
)

template<typename T>
constexpr auto operator--(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>{} and std::is_nothrow_move_constructible<T>{} and noexcept(--value)) {
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
