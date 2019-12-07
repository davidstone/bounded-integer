// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/arithmetic/divides.hpp>
#include <bounded/detail/arithmetic/modulus.hpp>
#include <bounded/detail/arithmetic/left_shift.hpp>
#include <bounded/detail/arithmetic/right_shift.hpp>
#include <bounded/detail/arithmetic/bitwise_and.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/arithmetic/unary_plus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/returns.hpp>

#include <operators/binary_minus.hpp>
#include <operators/compound_assignment.hpp>
#include <operators/increment_decrement.hpp>

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

constexpr auto operator++(auto & value) BOUNDED_RETURNS(
	value += constant<1>
)

constexpr auto operator--(auto & value) BOUNDED_RETURNS(
	value -= constant<1>
)

#define BOUNDED_COMMON_ARITHMETIC \
	OPERATORS_IMPORT_COMPOUND_ASSIGNMENT \
	using operators::binary::operator-; \
	using operators::postfix::operator++; \
	using operators::postfix::operator--; \
	using bounded::detail::arithmetic::operator++; \
	using bounded::detail::arithmetic::operator--;

}	// namespace arithmetic

BOUNDED_COMMON_ARITHMETIC

}	// namespace detail

BOUNDED_COMMON_ARITHMETIC

}	// namespace bounded
