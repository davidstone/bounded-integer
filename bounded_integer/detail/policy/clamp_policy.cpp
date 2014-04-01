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

#include "clamp_policy.hpp"
#include "../class.hpp"
#include "../comparison_operators.hpp"
#include "../literal.hpp"
#include "../numeric_limits.hpp"

namespace {
using namespace bounded::literal;

static constexpr intmax_t minimum = 27;
static constexpr intmax_t maximum = 567;
constexpr bounded::clamp_policy policy;
static_assert(policy.assignment(20, minimum, maximum) == minimum, "Failure to properly clamp lesser positive values.");
static_assert(policy.assignment(-25, minimum, maximum) == minimum, "Failure to properly clamp negative values to a positive value.");
static_assert(policy.assignment(1000, minimum, maximum) == maximum, "Failure to properly clamp greater positive values.");
static_assert(policy.assignment(2000_bi, minimum, maximum) == maximum, "Fail to clamp above range with a strictly greater type.");

using type = bounded::integer<-100, 100, bounded::clamp_policy>;
constexpr auto initial = std::numeric_limits<type::underlying_type>::max() + 1_bi;
constexpr type value(initial);
static_assert(value == std::numeric_limits<type>::max(), "Fail to clamp value when the source type is larger than the destination type.");


constexpr bounded::integer<minimum, maximum, bounded::clamp_policy> integer(1000_bi);
static_assert(integer == maximum, "Fail to clamp when using a bounded.");

}	// namespace
