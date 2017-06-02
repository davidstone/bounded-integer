// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/common_type.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/noexcept.hpp>

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename Min, typename Max>
struct min_max_t {
	Min min;
	Max max;
};

template<typename Min, typename Max>
constexpr auto min_max(Min min, Max max) BOUNDED_NOEXCEPT_VALUE(
	min_max_t<Min, Max>{std::move(min), std::move(max)}
)

}	// namespace detail

// It is safe to use the non_check constructor because we already know that the
// result will fit in result_t. We have to cast to the intermediate common_t in
// case result_t is narrower than one of the arguments.
#define BOUNDED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_range) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy, storage_type lhs_storage, bool lhs_poisoned, \
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy, storage_type rhs_storage, bool rhs_poisoned \
> \
constexpr auto operator symbol( \
	integer<lhs_min, lhs_max, lhs_policy, lhs_storage, lhs_poisoned> const lhs, \
	integer<rhs_min, rhs_max, rhs_policy, rhs_storage, rhs_poisoned> const rhs \
) noexcept { \
	constexpr auto range = operator_range(detail::min_max(lhs_min, lhs_max), detail::min_max(rhs_min, rhs_max)); \
	using result_t = integer< \
		range.min, \
		range.max, \
		common_policy_t<lhs_policy, rhs_policy>, \
		detail::common_storage_type(lhs_storage, rhs_storage), \
		lhs_poisoned or rhs_poisoned \
	>; \
	using common_t = typename std::common_type_t<result_t, std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>::underlying_type; \
	return result_t(static_cast<common_t>(lhs) symbol static_cast<common_t>(rhs), non_check); \
}

}	// namespace bounded

