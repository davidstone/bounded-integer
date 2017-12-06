// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
#include <bounded/detail/arithmetic/pointer.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison_builtin.hpp>
#include <bounded/detail/typedefs.hpp>
#include "../../homogeneous_equals.hpp"

namespace {

constexpr auto signed_max = bounded::constant<std::numeric_limits<std::intmax_t>::max()>;
constexpr auto signed_min = bounded::constant<std::numeric_limits<std::intmax_t>::min()>;
constexpr auto unsigned_max = bounded::constant<std::numeric_limits<std::uintmax_t>::max()>;

constexpr bounded::checked_integer<1, 10> const x(9, bounded::non_check);
constexpr bounded::checked_integer<-3, 11> const z(4, bounded::non_check);

// unary plus
static_assert(homogeneous_equals(
	+bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	+signed_max,
	signed_max
));
static_assert(homogeneous_equals(
	+signed_min,
	signed_min
));
static_assert(homogeneous_equals(
	+unsigned_max,
	unsigned_max
));
static_assert(homogeneous_equals(
	+x,
	x
));

// unary minus
static_assert(homogeneous_equals(
	-bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	-signed_max,
	bounded::constant<-static_cast<std::intmax_t>(signed_max)>
));
static_assert(homogeneous_equals(
	-signed_min,
	bounded::constant<-static_cast<std::uintmax_t>(signed_min)>
));
static_assert(homogeneous_equals(
	-x,
	bounded::checked_integer<-10, -1>(-9, bounded::non_check)
));

// plus
static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> + bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	signed_max + bounded::constant<1>,
	bounded::constant<static_cast<std::uintmax_t>(signed_max) + 1>
));
#if 0
static_assert(homogeneous_equals(
	unsigned_max + signed_min,
	bounded::constant<static_cast<std::uintmax_t>(unsigned_max) + static_cast<std::uintmax_t>(signed_min)>
));
#endif
static_assert(homogeneous_equals(
	x + z,
	bounded::checked_integer<-2, 21>(13, bounded::non_check)
));
static_assert(homogeneous_equals(
	x + std::integral_constant<int, 5>{},
	bounded::integer<6, 15>(14, bounded::non_check)
));

// minus
static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<1>,
	bounded::constant<-1>
));
#if 0
static_assert(homogeneous_equals(
	unsigned_max - signed_max,
	bounded::constant<static_cast<std::uintmax_t>(unsigned_max) - static_cast<std::uintmax_t>(signed_max)>
));
#endif
static_assert(homogeneous_equals(
	x - z,
	bounded::checked_integer<-10, 13>(5, bounded::non_check)
));

// multiplies
static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<1000>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<1>,
	signed_max
));
static_assert(homogeneous_equals(
	signed_min * bounded::constant<1>,
	signed_min
));
static_assert(homogeneous_equals(
	unsigned_max * bounded::constant<1>,
	unsigned_max
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<-1>,
	-signed_max
));
#if 0
static_assert(homogeneous_equals(
	signed_min * bounded::constant<-1>,
	-signed_min
));
#endif
static_assert(homogeneous_equals(
	signed_max * bounded::constant<2>,
	signed_max + signed_max
));
static_assert(homogeneous_equals(
	x * z,
	bounded::checked_integer<-30, 110>(36, bounded::non_check)
));

// divides
static_assert(homogeneous_equals(
	bounded::constant<0> / bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<2>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> / bounded::constant<-1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<-1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> / bounded::constant<1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> / bounded::constant<-1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	signed_max / bounded::constant<1>,
	signed_max
));
static_assert(homogeneous_equals(
	signed_min / bounded::constant<1>,
	signed_min
));
static_assert(homogeneous_equals(
	unsigned_max / bounded::constant<1>,
	unsigned_max
));
static_assert(homogeneous_equals(
	signed_max / bounded::constant<-1>,
	-signed_max
));
#if 0
static_assert(homogeneous_equals(
	signed_min / bounded::constant<-1>,
	-signed_min
));
#endif
static_assert(homogeneous_equals(
	unsigned_max / bounded::constant<2>,
	signed_max
));
static_assert(homogeneous_equals(
	unsigned_max / signed_max,
	bounded::constant<2>
));
static_assert(homogeneous_equals(
	x / z,
	bounded::checked_integer<-10, 10>(2, bounded::non_check)
));

// constexpr auto fails_to_compile = bounded::constant<1> / bounded::constant<0>;


// modulus
static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<11>,
	bounded::constant<10>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<9>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<9> % bounded::constant<11>,
	bounded::constant<9>
));
static_assert(homogeneous_equals(
	bounded::constant<11> % bounded::constant<9>,
	bounded::constant<2>
));
static_assert(homogeneous_equals(
	bounded::constant<13> % bounded::constant<6>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	signed_min % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	signed_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	unsigned_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<17, 23>(20, bounded::non_check) % bounded::integer<-54, -6>(-33, bounded::non_check),
	bounded::integer<0, 23>(20 % -33, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<-54, -6>(-33, bounded::non_check) % bounded::integer<17, 23>(20, bounded::non_check),
	bounded::integer<-22, 0>(-33 % 20, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<-22, 0>(-33 % 20, bounded::non_check) % bounded::integer<0, 23>(20 % -33, bounded::non_check),
	bounded::integer<-22, 0>(-13, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<0, 10>(10, bounded::non_check) % bounded::constant<6>,
	bounded::integer<0, 5>(4)
));
static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<73, 76>(73, bounded::non_check) % bounded::integer<7, 8>(7, bounded::non_check),
	bounded::integer<1, 6>(73 % 7)
));
static_assert(homogeneous_equals(
	bounded::integer<74, 75>(74, bounded::non_check) % bounded::integer<7, 8>(7, bounded::non_check),
	bounded::integer<2, 5>(74 % 7, bounded::non_check)
));

// auto undefined = 1 % zero;

#if 0	
// The algorithm to compute the bounds current runs in O(n) compile time,
// and thus this test exceed's the constexpr evaluation limits.
constexpr auto bounded_intmax_t = bounded::integer<std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()>(std::numeric_limits<intmax_t>::min());
static_assert(homogeneous_equals(
	bounded_intmax_t % bounded_intmax_t,
	bounded::integer<std::numeric_limits<intmax_t>::min() + 1, -(std::numeric_limits<intmax_t>::min() + 1)>(0)
));
#endif


// left shift
static_assert(homogeneous_equals(
	bounded::integer<0, 2>(1, bounded::non_check) << bounded::integer<0, 60>(3, bounded::non_check),
	bounded::integer<0, 2LL << 60LL>(1 << 3, bounded::non_check)
));

// right shift
static_assert(homogeneous_equals(
	bounded::constant<100> >> bounded::integer<0, 50>(1, bounded::non_check),
	bounded::integer<0, 100>(100 >> 1, bounded::non_check)
));


// bitwise and
static_assert((bounded::constant<17> & 1) == bounded::constant<1>);
static_assert((bounded::constant<18> & 1) == bounded::constant<0>);



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
