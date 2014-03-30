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

#include "common_type.hpp"

namespace {

using type1 = bounded_integer::bounded_integer<1, 5, bounded_integer::null_policy>;
using type2 = bounded_integer::bounded_integer<3, 10, bounded_integer::null_policy>;
using common_type2 = std::common_type_t<type1, type2>;
using expected_type2 = bounded_integer::bounded_integer<1, 10, bounded_integer::null_policy>;
static_assert(std::is_same<expected_type2, common_type2>::value, "common_type wrong for 2 values.");
using type3 = bounded_integer::bounded_integer<-5, -5, bounded_integer::null_policy>;
using common_type3 = std::common_type_t<type1, type2, type3>;
using expected_type3 = bounded_integer::bounded_integer<-5, 10, bounded_integer::null_policy>;
static_assert(std::is_same<expected_type3, common_type3>::value, "common_type wrong for 3 values.");
using type4 = bounded_integer::bounded_integer<0, 0, bounded_integer::null_policy>;
using common_type4 = std::common_type_t<type1, type2, type3, type4>;
using expected_type4 = bounded_integer::bounded_integer<-5, 10, bounded_integer::null_policy>;
static_assert(std::is_same<expected_type4, common_type4>::value, "common_type wrong for 4 values.");

}	// namespace
