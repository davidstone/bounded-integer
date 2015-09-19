// Verify that the header can stand on its own, run tests
// Copyright (C) 2015 David Stone
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

#include <containers/array/array.hpp>
#include <bounded_integer/bounded_integer.hpp>

namespace {
using namespace bounded::literal;

constexpr auto size = 5;
using array_type = containers::array<int, size>;
constexpr array_type a = {{}};
static_assert(containers::size(a) == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");
static_assert(at(a, size - 1) == 0, "Incorrect value with at.");
static_assert(a.end() - a.begin() == size, "Incorrect difference type.");


constexpr containers::array<int, 0> empty_array = {{}};
static_assert(empty_array.begin() == empty_array.end(), "Empty array.");
// static_assert(empty_array[0_bi] == 0, "Should not compile.");

}	// namespace

auto main() -> int {}
