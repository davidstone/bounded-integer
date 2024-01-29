// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOUNDED_ARITHMETIC_COMMON_ARITHMETIC_HPP
#define BOUNDED_ARITHMETIC_COMMON_ARITHMETIC_HPP

#include <operators/compound_assignment.hpp>

import bounded.arithmetic.increment_decrement;

import operators;

#define BOUNDED_COMMON_ARITHMETIC(...) \
	OPERATORS_IMPORT_COMPOUND_ASSIGNMENT(__VA_ARGS__) \
	__VA_ARGS__ using operators::binary::operator-; \
	__VA_ARGS__ using operators::postfix::operator++; \
	__VA_ARGS__ using operators::postfix::operator--; \
	__VA_ARGS__ using bounded::arithmetic::operator++; \
	__VA_ARGS__ using bounded::arithmetic::operator--;

#endif // BOUNDED_ARITHMETIC_COMMON_ARITHMETIC_HPP