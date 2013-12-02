// Verify that the header can stand on its own
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

#include "make_ranged.hpp"
#include "policy/throw_policy.hpp"

static_assert(std::is_same<detail::equivalent_overflow_policy<int>, null_policy>::value, "int should have a null_policy");
static_assert(std::is_same<detail::equivalent_overflow_policy<ranged_integer<0, 0, throw_policy>>, throw_policy>::value, "incorrect equivalent_overflow_policy for ranged_integer.");

