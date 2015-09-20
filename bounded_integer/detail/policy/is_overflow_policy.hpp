// Determines if a class meets the requirements of an overflow policy.
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

#pragma once

#include "../has_nested_type.hpp"

namespace bounded {
namespace detail {

BOUNDED_INTEGER_MAKE_NESTED_TYPE_TEST(overflow_policy_tag)

}	// namespace detail


template<typename overflow_policy>
struct is_overflow_policy : std::integral_constant<
	bool,
	detail::has_nested_type_overflow_policy_tag<overflow_policy>
>{};

}	// namespace bounded
