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

#include "range.hpp"
#include "literal.hpp"

namespace {
using namespace ::bounded_integer::literal;

constexpr auto x = bounded_integer::range(10_bi);
static_assert(
	*x.begin() == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	x.front() == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	x.back() == 9_bi,
	"Incorrect initial value of range."
);

static_assert(
	bounded_integer::range(5_bi, 12_bi)[2_bi] == 7_bi,
	"Incorrect indexed value of range that does not start with 0."
);

static_assert(
	bounded_integer::range(0_bi, 0_bi).size() == 0_bi,
	"Incorrect size of empty range."
);
static_assert(
	bounded_integer::range(-5_bi, -5_bi).empty(),
	"Incorrect size of empty range."
);

static_assert(
	bounded_integer::range(
		static_cast<bounded_integer::bounded_integer<1, 2, bounded_integer::null_policy>>(1_bi),
		static_cast<bounded_integer::bounded_integer<3, 5, bounded_integer::null_policy>>(3_bi)
	).size() == 2_bi,
	"Incorrect size of range with non-overlapping and possibly variable begin and end."
);

}	// namespace
