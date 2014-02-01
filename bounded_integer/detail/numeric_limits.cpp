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

#include "numeric_limits.hpp"

#include "class.hpp"
#include "comparison_operators.hpp"
#include "policy/null_policy.hpp"

namespace {

using policy = bounded_integer::null_policy;

static_assert(std::numeric_limits<bounded_integer::bounded_integer<1, 1000, policy>>::digits == 0, "Meaningless digits not 0.");

constexpr intmax_t min = 0;
constexpr intmax_t max = 714;
using dynamic_type = bounded_integer::bounded_integer<min, max, policy, bounded_integer::bounds::dynamic_max>;

constexpr dynamic_type value(max, bounded_integer::non_check);
static_assert(std::numeric_limits<dynamic_type>::min() == min, "Incorrect min for dynamic type.");
static_assert(std::numeric_limits<dynamic_type>::max() == max, "Incorrect max for dynamic type.");

}	// namespace
