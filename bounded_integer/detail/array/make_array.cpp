// Verify that the header can stand on its own, run tests
// Copyright (C) 2013 David Stone
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

#include "make_array.hpp"
#include "../comparison_operators.hpp"
#include "../literal.hpp"

namespace {

namespace check_array {

constexpr auto dynamic_int_array = make_array(0, 3, 6);
static_assert(dynamic_int_array.size() == 3, "Array size wrong.");
static_assert(dynamic_int_array.at(2) == 6, "Array element wrong.");
static_assert(std::is_same<int, decltype(dynamic_int_array)::value_type>::value, "Array element type wrong for all int arguments.");

constexpr auto bounded_array = make_array(-100_bi, 5_bi, 378_bi, 23_bi, 10000_bi);
static_assert(bounded_array.size() == 5, "Array size wrong.");
static_assert(bounded_array[2_bi] == 378, "Array element wrong.");
static_assert(
	std::is_same<bounded_integer<-100, 10000, null_policy>, decltype(bounded_array)::value_type>::value,
	"Array element type wrong for exact template arguments."
);

constexpr auto value = make_explicit_array<5, 4>(
	0_bi, 1_bi, 2_bi, 3_bi,
	4_bi, 5_bi, 6_bi, 7_bi,
	5_bi, 7_bi, 23_bi, 2474_bi,
	-6_bi, 2467_bi, 29_bi, -4573_bi,
	0_bi, 0_bi, 0_bi, 0_bi
);
static_assert(value.size() == 5, "First dimension wrong.");
static_assert(value[0_bi].size() == 4, "Second dimension wrong.");
static_assert(value[3_bi][1_bi] == 2467_bi, "Value wrong.");
using value_type = decltype(value)::value_type::value_type;
static_assert(std::numeric_limits<value_type>::min() == -4573, "min wrong");
static_assert(std::numeric_limits<value_type>::max() == 2474, "max wrong");

// First is deduced
static constexpr std::size_t second = 3;
static constexpr std::size_t third = 4;
static constexpr std::size_t fourth = 6;
constexpr auto four_dimensions = make_array<second, third, fourth>(
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
static_assert(four_dimensions.size() == 1, "First dimension wrong.");
static_assert(four_dimensions[0_bi].size() == second, "Second dimension wrong.");
static_assert(four_dimensions[0_bi][0_bi].size() == third, "Third dimension wrong.");
static_assert(four_dimensions[0_bi][0_bi][0_bi].size() == fourth, "Fourth dimension wrong.");

}	// namespace check_multi_array
}	// namespace
