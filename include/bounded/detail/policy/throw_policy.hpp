// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/policy/null_policy.hpp>
#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/is_poisoned.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/string.hpp>
#include <stdexcept>

namespace bounded {
namespace policy_detail {

using default_exception = std::range_error;

template<intmax_t minimum, intmax_t maximum, typename T>
constexpr auto reduce_range(T && value) BOUNDED_NOEXCEPT(
	integer<
		max(minimum, basic_numeric_limits<T>::min()),
		min(maximum, basic_numeric_limits<T>::max()),
		null_policy,
		storage_type::fast,
		bounded::detail::is_poisoned<T>
	>(std::forward<T>(value))
)

}	// namespace policy_detail

template<typename Exception = policy_detail::default_exception>
struct throw_policy {
	constexpr throw_policy() noexcept {}

	// TODO: Conditional noexcept
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) {
		static_assert(is_bounded_integer<Minimum>, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<Maximum>, "Only bounded::integer types are supported.");
		if (minimum <= value and value <= maximum) {
			return policy_detail::reduce_range<basic_numeric_limits<Minimum>::min(), basic_numeric_limits<Maximum>::max()>(std::forward<T>(value));
		}
		throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
	
	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded
