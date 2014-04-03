// Verify that the header can stand on its own
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

#include "conditional.hpp"
#include "comparison_operators.hpp"
#include "literal.hpp"
#include <type_traits>

namespace {

using namespace bounded::literal;

constexpr auto value = BOUNDED_INTEGER_CONDITIONAL(true, 7_bi, 9_bi);

static_assert(value == 7_bi, "Wrong conditional value.");
static_assert(std::is_same<decltype(value), bounded::integer<7, 9> const>::value, "Wrong conditional type.");

}	// namespace
