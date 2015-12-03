// Verify that the header can stand on its own, run tests
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <containers/array/make_array.hpp>

#include <bounded_integer/bounded_integer.hpp>

namespace {
using namespace bounded::literal;

constexpr auto dynamic_int_array = containers::make_array(0, 3, 6);
static_assert(size(dynamic_int_array) == 3, "Array size wrong.");
static_assert(at(dynamic_int_array, 2) == 6, "Array element wrong.");
static_assert(
	std::is_same<int, decltype(dynamic_int_array)::value_type>::value,
	"Array element type wrong for all int arguments."
);

constexpr auto bounded_array = containers::make_array(-100_bi, 5_bi, 378_bi, 23_bi, 10000_bi);
static_assert(size(bounded_array) == 5, "Array size wrong.");
static_assert(bounded_array[2_bi] == 378, "Array element wrong.");
static_assert(
	std::is_same<
		bounded::integer<-100, 10000>,
		decltype(bounded_array)::value_type
	>::value,
	"Array element type wrong for exact template arguments."
);

constexpr auto value = containers::make_explicit_array<5, 4>(
	0_bi, 1_bi, 2_bi, 3_bi,
	4_bi, 5_bi, 6_bi, 7_bi,
	5_bi, 7_bi, 23_bi, 2474_bi,
	-6_bi, 2467_bi, 29_bi, -4573_bi,
	0_bi, 0_bi, 0_bi, 0_bi
);
static_assert(size(value) == 5, "First dimension wrong.");
static_assert(size(value[0_bi]) == 4, "Second dimension wrong.");
static_assert(value[3_bi][1_bi] == 2467_bi, "Value wrong.");
using value_type = decltype(value)::value_type::value_type;
static_assert(std::numeric_limits<value_type>::min() == -4573, "min wrong");
static_assert(std::numeric_limits<value_type>::max() == 2474, "max wrong");

// First is deduced
static constexpr std::size_t second = 3;
static constexpr std::size_t third = 4;
static constexpr std::size_t fourth = 6;
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
static_assert(size(four_dimensions) == 1, "First dimension wrong.");
static_assert(size(four_dimensions[0_bi]) == second, "Second dimension wrong.");
static_assert(size(four_dimensions[0_bi][0_bi]) == third, "Third dimension wrong.");
static_assert(size(four_dimensions[0_bi][0_bi][0_bi]) == fourth, "Fourth dimension wrong.");

using explicit_type = bounded::integer<0, 1, bounded::throw_policy<>>;
constexpr auto typed_array = containers::make_array<explicit_type>(0_bi);
static_assert(size(typed_array) == 1_bi, "Incorrect size with explicit type.");
static_assert(std::is_same<decltype(typed_array)::value_type, explicit_type>::value, "Incorrect type with explicit type.");
static_assert(front(typed_array) == 0_bi, "Incorrect value with explicit type.");


constexpr auto array_n = containers::make_array_n(6_bi, 5);
static_assert(std::is_same<std::decay_t<decltype(array_n)>::value_type, int>::value, "Incorrect type from make_array_n.");
static_assert(size(array_n) == 6_bi, "Incorrect size from make_array_n.");
static_assert(array_n[3_bi] == 5, "Incorrect values from make_array_n.");

struct non_copyable {
	constexpr non_copyable(non_copyable const &) = delete;
	constexpr non_copyable(non_copyable &&) = default;
};

constexpr auto array_non_copyable = containers::make_array_n(1_bi, non_copyable{});
static_assert(size(array_non_copyable) == 1_bi, "Incorrect array size of 1 for move-only types.");

constexpr auto array_empty = containers::make_array_n(0_bi, 2);
static_assert(empty(array_empty), "Incorrect array size for empty array.");

constexpr auto array_non_copyable_empty = containers::make_array_n(0_bi, non_copyable{});
static_assert(empty(array_non_copyable_empty), "Incorrect array size for empty array of move-only.");

}	// namespace
