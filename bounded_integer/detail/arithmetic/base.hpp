// Arithmetic operations base
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

#include "../forward_declaration.hpp"
#include "../noexcept.hpp"
#include "../policy/common_policy.hpp"

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename Min, typename Max>
struct min_max_t {
	template<typename Minimum, typename Maximum>
	constexpr min_max_t(Minimum && min_, Maximum && max_) noexcept(noexcept(std::is_nothrow_constructible<Min, Minimum &&>::value and std::is_nothrow_constructible<Max, Maximum &&>::value)):
		min(std::forward<Minimum>(min_)),
		max(std::forward<Maximum>(max_)) {
	}
	Min min;
	Max max;
};

template<typename Min, typename Max>
constexpr auto min_max(Min && min, Max && max) BOUNDED_NOEXCEPT(
	(min_max_t<std::remove_cv_t<std::remove_reference_t<Min>>, std::remove_cv_t<std::remove_reference_t<Max>>>(std::forward<Min>(min), std::forward<Max>(max)))
)

}	// namespace detail

// TODO: consider how dynamic bounds fit into this

// It is safe to use the non_check constructor because we already know that the
// result will fit in result_t. We have to cast to the intermediate common_t in
// case result_t is narrower than one of the arguments.
#define BOUNDED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_name) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy, \
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy, \
	storage_type storage \
> \
constexpr auto operator symbol( \
	integer<lhs_min, lhs_max, lhs_policy, storage> const lhs, \
	integer<rhs_min, rhs_max, rhs_policy, storage> const rhs \
) noexcept { \
	using result_t = integer< \
		operator_range(detail::min_max(lhs_min, lhs_max), detail::min_max(rhs_min, rhs_max), operator_name{}).min, \
		operator_range(detail::min_max(lhs_min, lhs_max), detail::min_max(rhs_min, rhs_max), operator_name{}).max, \
		common_policy_t<lhs_policy, rhs_policy>, \
		storage \
	>; \
	using common_t = typename std::common_type_t<result_t, std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>::underlying_type; \
	return result_t(static_cast<common_t>(lhs) symbol static_cast<common_t>(rhs), non_check); \
}

}	// namespace bounded

