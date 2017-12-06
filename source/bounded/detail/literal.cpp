// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/literal.hpp>

#include "../homogeneous_equals.hpp"

namespace {

using namespace bounded::literal;

// I have to use the preprocessor here to create an integer literal
#define BOUNDED_INTEGER_CHECK_LITERAL(x) \
	static_assert(homogeneous_equals( \
		x ## _bi, \
		bounded::constant<x> \
	))

BOUNDED_INTEGER_CHECK_LITERAL(0);
BOUNDED_INTEGER_CHECK_LITERAL(1);
BOUNDED_INTEGER_CHECK_LITERAL(10);
BOUNDED_INTEGER_CHECK_LITERAL(1000);
BOUNDED_INTEGER_CHECK_LITERAL(4294967295);
BOUNDED_INTEGER_CHECK_LITERAL(4294967296);
BOUNDED_INTEGER_CHECK_LITERAL(9223372036854775807);
#undef BOUNDED_INTEGER_CHECK_LITERAL

}	// namespace
