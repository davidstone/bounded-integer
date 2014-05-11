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

#ifndef BOUNDED_INTEGER_CONDITIONAL_HPP_
#define BOUNDED_INTEGER_CONDITIONAL_HPP_

#include "common_type.hpp"

#define BOUNDED_CONDITIONAL(condition, lhs, rhs) \
	((condition) ? \
		static_cast<std::common_type_t<decltype(lhs), decltype(rhs)>>(lhs) : \
		static_cast<std::common_type_t<decltype(lhs), decltype(rhs)>>(rhs))

#endif	// BOUNDED_INTEGER_CONDITIONAL_HPP_
