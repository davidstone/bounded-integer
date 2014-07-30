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
#include "../literal.hpp"
#include "../operators/comparison.hpp"

namespace {
using namespace bounded::literal;

constexpr auto dynamic_optional_array = bounded::make_optional_array(0, bounded::none, 3, 6);
static_assert(
	dynamic_optional_array.size() == 4,
	"Array size wrong."
);
static_assert(
	*dynamic_optional_array[3_bi] == 6,
	"valued element wrong."
);
static_assert(
	dynamic_optional_array[1_bi] == bounded::none,
	"none_t element wrong."
);
static_assert(
	std::is_same<bounded::optional<int>, decltype(dynamic_optional_array)::value_type>::value,
	"Array element type wrong for mixed int + none_t arguments."
);

static_assert(
	std::is_same<bounded::optional<int>, decltype(bounded::make_optional_array(0))::value_type>::value,
	"optional array type wrong with no missing values."
);


constexpr auto known_optional_array = bounded::make_optional_array(0_bi, bounded::none, 3_bi, 6_bi);
static_assert(
	known_optional_array.size() == 4,
	"Array size wrong."
);
static_assert(
	*known_optional_array[3_bi] == 6_bi,
	"valued element wrong."
);
static_assert(
	known_optional_array[1_bi] == bounded::none,
	"none_t element wrong."
);
static_assert(
	std::is_same<bounded::optional<bounded::integer<0, 6>>, decltype(known_optional_array)::value_type>::value,
	"Array element type wrong for mixed bounded + none_t arguments."
);

constexpr auto none_first_optional_array = bounded::make_optional_array(bounded::none, 0);
static_assert(
	none_first_optional_array[0_bi] == bounded::none,
	"none_t element wrong."
);
constexpr auto none_last_optional_array = bounded::make_optional_array(0, bounded::none);
static_assert(
	none_last_optional_array[1_bi] == bounded::none,
	"none_t element wrong."
);

}	// namespace
