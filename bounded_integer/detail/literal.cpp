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

#include "literal.hpp"
#include "comparison_operators.hpp"

namespace bounded_integer {
namespace {
using namespace literal;

// I have to use the preprocessor here to create an integer literal
#define BOUNDED_INTEGER_CHECK_LITERAL(x) \
	static_assert(std::numeric_limits<decltype(x ## _bi)>::min() == std::numeric_limits<decltype(x ## _bi)>::max(), "Literal does not have a min possible value equal to a max possible value."); \
	static_assert(std::numeric_limits<decltype(x ## _bi)>::min() == x ## _bi, "Literal does not have a value equal to the range."); \
	\
	static_assert(x ## _bi == static_cast<decltype(x ## _bi)::underlying_type>(x), "Inaccurate value of " #x " (cast x)"); \
	static_assert(static_cast<decltype(x)>(x ## _bi) == x, "Inaccurate value of " #x " (cast value)");

BOUNDED_INTEGER_CHECK_LITERAL(0)
BOUNDED_INTEGER_CHECK_LITERAL(1)
BOUNDED_INTEGER_CHECK_LITERAL(10)
BOUNDED_INTEGER_CHECK_LITERAL(1000)
BOUNDED_INTEGER_CHECK_LITERAL(4294967295)
BOUNDED_INTEGER_CHECK_LITERAL(4294967296)
BOUNDED_INTEGER_CHECK_LITERAL(9223372036854775807)
BOUNDED_INTEGER_CHECK_LITERAL(-1)
BOUNDED_INTEGER_CHECK_LITERAL(-0)
#undef BOUNDED_INTEGER_CHECK_LITERAL

}	// namespace
}	// namespace bounded_integer
