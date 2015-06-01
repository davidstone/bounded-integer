// This fakes a ternary conditional operator overload
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

#pragma once

#include "common_type_and_value_category.hpp"

#define BOUNDED_CONDITIONAL(condition, lhs, rhs) \
	((condition) ? \
		static_cast<bounded::detail::common_type_and_value_category_t<decltype(lhs), decltype(rhs)>>(lhs) : \
		static_cast<bounded::detail::common_type_and_value_category_t<decltype(lhs), decltype(rhs)>>(rhs))

