// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/array/make_array.hpp>

#include <containers/at.hpp>
#include <containers/front_back.hpp>
#include <containers/is_empty.hpp>

#include <bounded/integer.hpp>

namespace {
using namespace bounded::literal;

constexpr auto dynamic_int_array = containers::make_array(0, 3, 6);
static_assert(containers::size(dynamic_int_array) == 3, "Array size wrong.");
static_assert(containers::at(dynamic_int_array, 2) == 6, "Array element wrong.");
static_assert(
	std::is_same_v<int, decltype(dynamic_int_array)::value_type>,
	"Array element type wrong for all int arguments."
);

constexpr auto bounded_array = containers::make_array(-100_bi, 5_bi, 378_bi, 23_bi, 10000_bi);
static_assert(containers::size(bounded_array) == 5, "Array size wrong.");
static_assert(bounded_array[2_bi] == 378, "Array element wrong.");
static_assert(bounded::min_value<decltype(bounded_array)::value_type> == -100_bi);
static_assert(bounded::max_value<decltype(bounded_array)::value_type> == 10000_bi);

constexpr auto value = containers::make_explicit_array<5, 4>(
	0_bi, 1_bi, 2_bi, 3_bi,
	4_bi, 5_bi, 6_bi, 7_bi,
	5_bi, 7_bi, 23_bi, 2474_bi,
	-6_bi, 2467_bi, 29_bi, -4573_bi,
	0_bi, 0_bi, 0_bi, 0_bi
);
static_assert(containers::size(value) == 5, "First dimension wrong.");
static_assert(containers::size(value[0_bi]) == 4, "Second dimension wrong.");
static_assert(value[3_bi][1_bi] == 2467_bi, "Value wrong.");
using value_type = decltype(value)::value_type::value_type;
static_assert(bounded::min_value<value_type> == -4573, "min wrong");
static_assert(bounded::max_value<value_type> == 2474, "max wrong");

// First is deduced
constexpr std::size_t second = 3;
constexpr std::size_t third = 4;
constexpr std::size_t fourth = 6;
constexpr auto four_dimensions = containers::make_array<second, third, fourth>(
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,

	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,

	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi,
	0_bi, 1_bi, 2_bi, 3_bi, 4_bi, 5_bi
);
static_assert(containers::size(four_dimensions) == 1, "First dimension wrong.");
static_assert(containers::size(four_dimensions[0_bi]) == second, "Second dimension wrong.");
static_assert(containers::size(four_dimensions[0_bi][0_bi]) == third, "Third dimension wrong.");
static_assert(containers::size(four_dimensions[0_bi][0_bi][0_bi]) == fourth, "Fourth dimension wrong.");

using explicit_type = bounded::integer<0, 1>;
constexpr auto typed_array = containers::make_array<explicit_type>(0_bi);
static_assert(containers::size(typed_array) == 1_bi, "Incorrect size with explicit type.");
static_assert(std::is_same_v<decltype(typed_array)::value_type, explicit_type>, "Incorrect type with explicit type.");
static_assert(containers::front(typed_array) == 0_bi, "Incorrect value with explicit type.");


constexpr auto array_n = containers::make_array_n(6_bi, 5);
static_assert(std::is_same_v<std::decay_t<decltype(array_n)>::value_type, int>, "Incorrect type from make_array_n.");
static_assert(containers::size(array_n) == 6_bi, "Incorrect size from make_array_n.");
static_assert(array_n[3_bi] == 5, "Incorrect values from make_array_n.");

struct non_copyable {
	non_copyable() = default;
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
};

constexpr auto array_non_copyable = containers::make_array_n(1_bi, non_copyable());
static_assert(containers::size(array_non_copyable) == 1_bi);

constexpr auto array_empty = containers::make_array_n(0_bi, 2);
static_assert(containers::is_empty(array_empty));

constexpr auto array_non_copyable_empty = containers::make_array_n(0_bi, non_copyable());
static_assert(containers::is_empty(array_non_copyable_empty));

constexpr auto large_size = 10000_bi;
constexpr auto large_array_n = containers::make_array_n(large_size, 0);
static_assert(containers::size(large_array_n) == large_size);

}	// namespace
