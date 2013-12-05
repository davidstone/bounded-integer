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

#include "make_ranged_array.hpp"
#include "comparison_operators.hpp"
#include "literal.hpp"

namespace {

namespace check_array {

constexpr auto dynamic_int_array = make_ranged_array(0, 3, 6);
static_assert(dynamic_int_array.size() == 3, "Array size wrong.");
static_assert(dynamic_int_array[2] == 6, "Array element wrong.");
static_assert(std::is_same<equivalent_type<int>, decay_t<decltype(dynamic_int_array[0])>>::value, "Array element type wrong for all int arguments.");

constexpr auto dynamic_mixed_array = make_ranged_array(-6, 15u);
static_assert(dynamic_mixed_array.size() == 2, "Array size wrong.");
static_assert(dynamic_mixed_array[0] == -6, "Array element wrong.");
static_assert(
	std::is_same<
		common_type_t<equivalent_type<int>, equivalent_type<unsigned>>,
		decay_t<decltype(dynamic_mixed_array[0])>
	>::value,
	"Array element type wrong for mixed int / unsigned arguments."
);

constexpr auto exact_array = make_ranged_array<-100, 5, 378, 23, 10000>();
static_assert(exact_array.size() == 5, "Array size wrong.");
static_assert(exact_array[2] == 378, "Array element wrong.");
static_assert(std::is_same<ranged_integer<-100, 10000, null_policy>, decay_t<decltype(exact_array[0])>>::value, "Array element type wrong for exact template arguments.");

}	// namespace check_array


namespace check_multi_array {

constexpr auto value = multi_array<5, 4>::make_explicit(
	0_ri, 1_ri, 2_ri, 3_ri,
	4_ri, 5_ri, 6_ri, 7_ri,
	5_ri, 7_ri, 23_ri, 2474_ri,
	-6_ri, 2467_ri, 29_ri, -4573_ri,
	0_ri, 0_ri, 0_ri, 0_ri
);
static_assert(value.size() == 5, "First dimension wrong.");
static_assert(value[0].size() == 4, "Second dimension wrong.");
static_assert(value[3][1] == 2467_ri, "Value wrong.");
using value_type = decltype(value)::value_type::value_type;
static_assert(std::numeric_limits<value_type>::min() == -4573, "min wrong");
static_assert(std::numeric_limits<value_type>::max() == 2474, "max wrong");

// First is deduced
static constexpr std::size_t second = 3;
static constexpr std::size_t third = 4;
static constexpr std::size_t fourth = 6;
constexpr auto four_dimensions = multi_array<second, third, fourth>::make_deduced(
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,

	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,

	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri,
	0_ri, 1_ri, 2_ri, 3_ri, 4_ri, 5_ri
);
static_assert(four_dimensions.size() == 1, "First dimension wrong.");
static_assert(four_dimensions[0].size() == second, "Second dimension wrong.");
static_assert(four_dimensions[0][0].size() == third, "Third dimension wrong.");
static_assert(four_dimensions[0][0][0].size() == fourth, "Fourth dimension wrong.");

}	// namespace check_multi_array

}	// namespace
