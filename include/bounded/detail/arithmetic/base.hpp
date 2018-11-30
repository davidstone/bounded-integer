// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/common_type.hpp>
#include <bounded/detail/noexcept.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename Min, typename Max>
struct min_max {
	Min min;
	Max max;
};
template<typename Min, typename Max>
min_max(Min min, Max max) -> min_max<Min, Max>;

template<
	auto lhs_min, auto lhs_max, typename lhs_policy,
	auto rhs_min, auto rhs_max, typename rhs_policy,
	typename ArithmeticFunction,
	typename OperatorRangeFunction
>
constexpr auto operator_overload(
	integer<lhs_min, lhs_max, lhs_policy> const lhs,
	integer<rhs_min, rhs_max, rhs_policy> const rhs,
	ArithmeticFunction const arithmetic_function,
	OperatorRangeFunction const operator_range
) noexcept {
	constexpr auto range = operator_range(
		min_max{constant<lhs_min>, constant<lhs_max>},
		min_max{constant<rhs_min>, constant<rhs_max>}
	);
	using result_t = integer<
		normalize<range.min>,
		normalize<range.max>,
		common_policy<lhs_policy, rhs_policy>
	>;
	using common_t = typename std::common_type_t<result_t, std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>::underlying_type;
	// It is safe to use the non_check constructor because we already know that
	// the result will fit in result_t. We have to cast to the intermediate
	// common_t in case result_t is narrower than one of the arguments.
	return result_t(arithmetic_function(static_cast<common_t>(lhs), static_cast<common_t>(rhs)), non_check);
}

} // namespace detail
} // namespace bounded
