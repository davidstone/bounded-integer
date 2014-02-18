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

#include "dynamic_policy.hpp"
#include "clamp_policy.hpp"
#include "throw_policy.hpp"

using namespace bounded_integer;

namespace {

using policy_type = dynamic_policy<0, 10, clamp_policy>;
constexpr policy_type policy;
static_assert(policy.assignment(3, 2, 5) == 3, "Incorrect dynamic policy result when the static range is entirely within range.");
static_assert(policy.assignment(11, 0, 20) == 10, "Incorrect dynamic clamp policy result when the dynamic range is the limiting factor.");

}	// namespace
