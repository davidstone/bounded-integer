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

#include "comparison_operators.hpp"

#include "../literal.hpp"

namespace {
using namespace bounded_integer::literal;

using integer = bounded_integer::integer<0, 10, bounded_integer::null_policy>;

constexpr bounded_integer::optional<integer> uninitialized;

static_assert(uninitialized == bounded_integer::none, "Fail to compare uninitialized optional to none_t ==.");
static_assert(uninitialized != 0_bi, "Fail to compare uninitialized optional to a value with !=.");
static_assert(uninitialized < -1_bi, "Fail to compare uninitialized optional to a value with <.");


constexpr bounded_integer::optional<integer> initialized(5_bi);

static_assert(initialized != bounded_integer::none, "Fail to compare initialized optional to none_t !=.");
static_assert(initialized == 5_bi, "Fail to compare initialized optional to a value with ==.");
static_assert(initialized != 4_bi, "Fail to compare initialized optional to a value with !=.");
static_assert(initialized < 100_bi, "Fail to compare initialized optional to a value with <.");
static_assert(initialized > -5_bi, "Fail to compare initialized optional to a value with <.");
static_assert(initialized <= 5_bi, "Fail to compare initialized optional to a value with <=.");
static_assert(initialized <= 6_bi, "Fail to compare initialized optional to a value with <=.");
static_assert(initialized >= 5_bi, "Fail to compare initialized optional to a value with >=.");
static_assert(initialized >= 0_bi, "Fail to compare initialized optional to a value with >=.");

static_assert(bounded_integer::none != initialized, "Fail to compare initialized optional to none_t !=.");
static_assert(5_bi == initialized, "Fail to compare initialized optional to a value with ==.");
static_assert(17_bi != initialized, "Fail to compare initialized optional to a value with !=.");
static_assert(2_bi < initialized, "Fail to compare initialized optional to a value with <.");
static_assert(123456 > initialized, "Fail to compare initialized optional to a value with >.");
static_assert(5_bi <= initialized, "Fail to compare initialized optional to a value with <=.");
static_assert(-25 <= initialized, "Fail to compare initialized optional to a value with <=.");
static_assert(5_bi >= initialized, "Fail to compare initialized optional to a value with >=.");
static_assert(300U >= initialized, "Fail to compare initialized optional to a value with >=.");

}	// namespace
