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

#include "null_policy.hpp"

namespace {

constexpr null_policy policy;
constexpr auto value1 = policy.assignment<0, 10>(5);
// This should not compile
// constexpr auto value2 = policy.assignment<0, 10>(15);
// This should
auto value3 = policy.assignment<0, 10>(15);

}	// namespace
