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

#include "make_ranged_optional_array.hpp"
#include "comparison_operators.hpp"
#include "literal.hpp"

namespace {

constexpr auto dynamic_optional_array = make_ranged_optional_array(0, none, 3, 6);
static_assert(dynamic_optional_array.size() == 4, "Array size wrong.");
static_assert(*dynamic_optional_array[3] == 6, "valued element wrong.");
static_assert(dynamic_optional_array[1] == none, "none_t element wrong.");
static_assert(std::is_same<optional<decltype(make_ranged(0))>, decay_t<decltype(dynamic_optional_array[0])>>::value, "Array element type wrong for mixed int + none_t arguments.");

static_assert(std::is_same<optional<decltype(make_ranged(0))>, decay_t<decltype(make_ranged_optional_array(0)[0])>>::value, "optional array type wrong with no missing values.");


constexpr auto known_optional_array = make_ranged_optional_array(0_ri, none, 3_ri, 6_ri);
static_assert(known_optional_array.size() == 4, "Array size wrong.");
static_assert(*known_optional_array[3] == 6_ri, "valued element wrong.");
static_assert(known_optional_array[1] == none, "none_t element wrong.");
static_assert(std::is_same<optional<ranged_integer<0, 6, null_policy>>, decay_t<decltype(known_optional_array[0])>>::value, "Array element type wrong for mixed ranged_integer + none_t arguments.");

constexpr auto none_first_optional_array = make_ranged_optional_array(none, 0);
static_assert(none_first_optional_array[0] == none, "none_t element wrong.");
constexpr auto none_last_optional_array = make_ranged_optional_array(0, none);
static_assert(none_last_optional_array[1] == none, "none_t element wrong.");

}	// namespace

