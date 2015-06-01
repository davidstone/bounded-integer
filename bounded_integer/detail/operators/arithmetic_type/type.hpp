// result_type of arithmetic operations
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

#include "plus.hpp"
#include "minus.hpp"
#include "multiplies.hpp"
#include "divides.hpp"
#include "modulus.hpp"
#include "left_shift.hpp"
#include "right_shift.hpp"

#include "../../policy/common_policy.hpp"

namespace bounded {
namespace detail {

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow,
	storage_type storage,
	typename Operator
>
using operator_result = integer<
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::min(),
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::max(),
	common_policy_t<lhs_overflow, rhs_overflow>,
	storage
>;

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
using unary_minus_result = integer<-maximum, -minimum, overflow_policy, storage>;

}	// namespace detail
}	// namespace bounded

