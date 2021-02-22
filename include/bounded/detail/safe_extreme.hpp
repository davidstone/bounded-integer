// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/comparison.hpp>

#include <type_traits>

namespace bounded::detail {

template<typename Limited>
constexpr auto safe_extreme(auto const pick_lhs, auto const lhs_, auto const rhs_) {
	auto normalized = [](auto const value) {
		using normalized_t = std::conditional_t<
			signed_builtin<std::decay_t<decltype(value)>>,
			max_signed_t,
			max_unsigned_t
		>;
		return static_cast<normalized_t>(value);
	};
	auto const lhs = normalized(lhs_);
	auto const rhs = normalized(rhs_);
	using result_type = std::conditional_t<std::is_same_v<decltype(lhs), decltype(rhs)>, decltype(lhs), Limited>;
	return (pick_lhs(bounded::detail::safe_compare(lhs, rhs))) ?
		static_cast<result_type>(lhs) :
		static_cast<result_type>(rhs);
}

template<typename Limited>
constexpr auto safe_extreme(auto const pick_lhs, auto const lhs, auto const rhs, auto const ... rest) {
	return bounded::detail::safe_extreme<Limited>(
		pick_lhs,
		bounded::detail::safe_extreme<Limited>(pick_lhs, lhs, rhs),
		rest...
	);
}

constexpr auto safe_min(auto... values) {
	return bounded::detail::safe_extreme<max_signed_t>(
		[](auto const cmp) { return cmp <= 0; },
		values...
	);
}

constexpr auto safe_max(auto... values) {
	return bounded::detail::safe_extreme<max_unsigned_t>(
		[](auto const cmp) { return cmp > 0; },
		values...
	);
}

} // namespace bounded::detail