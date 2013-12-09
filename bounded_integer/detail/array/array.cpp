// Verify that the header can stand on its own, run tests
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

#include "array.hpp"
#include "../comparison_operators.hpp"
#include "../literal.hpp"

namespace {

constexpr array<int, 5> a = {{}};
static_assert(a.size() == 5, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");
static_assert(a.at(4) == 0, "Incorrect value with at.");

}	// namespace
