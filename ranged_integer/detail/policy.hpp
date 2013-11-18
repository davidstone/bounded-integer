// Ranged integer policies that determine what to do on overflow
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

#ifndef RANGED_INTEGER_POLICY_HPP_
#define RANGED_INTEGER_POLICY_HPP_

// Default constructors must be provided to work around
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#253

#include "policy/clamp_policy.hpp"
#include "policy/null_policy.hpp"
#include "policy/throw_policy.hpp"

#endif	// RANGED_INTEGER_POLICY_HPP_
