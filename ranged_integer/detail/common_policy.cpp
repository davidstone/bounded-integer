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

#include "common_policy.hpp"

static_assert(
	std::is_same<common_policy_t<null_policy, null_policy>, null_policy>::value,
	"common_policy gives wrong type for all null_policy"
);

static_assert(
	std::is_same<common_policy_t<null_policy, throw_policy>, throw_policy>::value,
	"common_policy gives wrong type for one null_policy (first)"
);

static_assert(
	std::is_same<common_policy_t<throw_policy, null_policy>, throw_policy>::value,
	"common_policy gives wrong type for one null_policy (second)"
);

static_assert(
	std::is_same<common_policy_t<throw_policy, throw_policy>, throw_policy>::value,
	"common_policy gives wrong type for all throw_policy"
);
