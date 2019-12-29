// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/pointer.hpp>
#include <bounded/detail/class.hpp>

namespace {

using array_type = int[5];
constexpr array_type array{ 0, 1, 2, 3, 4 };

static_assert(
	*(std::begin(array) + bounded::constant<0>) == 0,
	"Incorrect pointer arithmetic with bounded::integer."
);
static_assert(
	*(array + bounded::constant<0>) == 0,
	"Incorrect array arithmetic with bounded::integer."
);
static_assert(
	array + bounded::constant<5> == std::end(array),
	"Incorrect array arithmetic with bounded::integer."
);


#if 0
// TODO: Test that this does not compile, improve error message
array + bounded::constant<6>
#endif

// Oops, not possible to overload array index operator
#if 0
static_assert(
	array[bounded::constant<0>] == 0,
	"Incorrect array indexing with bounded::integer."
);
#endif

}	// namespace
