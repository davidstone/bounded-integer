// Verify that the header can stand on its own, run tests
// Copyright (C) 2014 David Stone
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

#include "make_optional_array.hpp"
#include "comparison_operators.hpp"
#include "../comparison_operators.hpp"
#include "../literal.hpp"

namespace {
using namespace bounded_integer::literal;

constexpr auto dynamic_optional_array = bounded_integer::make_optional_array(0, bounded_integer::none, 3, 6);
static_assert(
	dynamic_optional_array.size() == 4,
	"Array size wrong."
);
static_assert(
	*dynamic_optional_array[3_bi] == 6,
	"valued element wrong."
);
static_assert(
	dynamic_optional_array[1_bi] == bounded_integer::none,
	"none_t element wrong."
);
static_assert(
	std::is_same<bounded_integer::optional<int>, decltype(dynamic_optional_array)::value_type>::value,
	"Array element type wrong for mixed int + none_t arguments."
);

static_assert(
	std::is_same<bounded_integer::optional<int>, decltype(bounded_integer::make_optional_array(0))::value_type>::value,
	"optional array type wrong with no missing values."
);


constexpr auto known_optional_array = bounded_integer::make_optional_array(0_bi, bounded_integer::none, 3_bi, 6_bi);
static_assert(
	known_optional_array.size() == 4,
	"Array size wrong."
);
static_assert(
	*known_optional_array[3_bi] == 6_bi,
	"valued element wrong."
);
static_assert(
	known_optional_array[1_bi] == bounded_integer::none,
	"none_t element wrong."
);
static_assert(
	std::is_same<bounded_integer::optional<bounded_integer::integer<0, 6, bounded_integer::null_policy>>, decltype(known_optional_array)::value_type>::value,
	"Array element type wrong for mixed bounded_integer + none_t arguments."
);

constexpr auto none_first_optional_array = bounded_integer::make_optional_array(bounded_integer::none, 0);
static_assert(
	none_first_optional_array[0_bi] == bounded_integer::none,
	"none_t element wrong."
);
constexpr auto none_last_optional_array = bounded_integer::make_optional_array(0, bounded_integer::none);
static_assert(
	none_last_optional_array[1_bi] == bounded_integer::none,
	"none_t element wrong."
);

}	// namespace
