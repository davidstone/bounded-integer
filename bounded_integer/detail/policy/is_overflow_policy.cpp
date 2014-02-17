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

#include "is_overflow_policy.hpp"
#include "all.hpp"
#include <type_traits>

namespace bounded_integer {

static_assert(is_overflow_policy<null_policy>::value, "null_policy not recognized as an overflow policy.");
static_assert(is_overflow_policy<throw_policy>::value, "throw_policy not recognized as an overflow policy.");
static_assert(is_overflow_policy<clamp_policy>::value, "clamp_policy not recognized as an overflow policy.");
static_assert(!is_overflow_policy<int>::value, "int recognized as an overflow policy.");

static_assert(std::is_constructible<null_policy, clamp_policy>::value, "Cannot construct one policy from another.");

}	// namespace bounded_integer
