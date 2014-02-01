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

#include "overlapping_range.hpp"
#include "class.hpp"
#include "policy/null_policy.hpp"

namespace {

using type = bounded_integer::bounded_integer<0, 0, bounded_integer::null_policy>;
static_assert(bounded_integer::detail::type_overlaps_range<type>(0, 0), "Type should overlap its own range.");
static_assert(bounded_integer::detail::type_fits_in_range<type>(0, 0), "Type should fit in its own range.");
static_assert(!bounded_integer::detail::type_overlaps_range<type>(1, 1), "Type should not overlap a disjoint range.");


}	// namespace
