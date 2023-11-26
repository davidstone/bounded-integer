// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.pointer;

import bounded.arithmetic.plus;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.integer;

import numeric_traits;
import std_module;

namespace bounded {

// This is faster to compile than `std::is_pointer_v`
template<typename>
constexpr auto is_pointer = false;

template<typename T>
constexpr auto is_pointer<T *> = true;

export template<typename T, bounded_integer Integer> requires(
	(is_pointer<T> or (std::is_array_v<T> and numeric_traits::max_value<Integer> <= constant<std::extent_v<T>>))
)
constexpr auto operator+(T const & array, Integer const number) {
	return array + number.value();
}


export template<bounded_integer Integer, typename T> requires(
	(is_pointer<T> or (std::is_array_v<T> and numeric_traits::max_value<Integer> <= constant<std::extent_v<T>>))
)
constexpr auto operator+(Integer const number, T const & array) {
	return number.value() + array;
}

// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

} // namespace bounded

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
