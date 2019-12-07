// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/conditional.hpp>
#include <bounded/detail/policy/throw_policy.hpp>
#include <bounded/detail/class.hpp>

#include <algorithm>
#include <stdexcept>
#include <string_view>

namespace bounded {
namespace detail {

template<auto maximum>
constexpr auto to_integer_positive_impl(std::string_view str) {
	using policy = throw_policy<std::invalid_argument>;
	auto positive_result = integer<0, maximum, policy>(constant<0>);
	for (auto const c : str) {
		positive_result *= constant<10>;
		// TODO: throw an exception type that mentions a bad character?
		positive_result += integer<0, 9, policy>(c - '0');
	}
	return positive_result;
}

template<auto minimum, auto maximum>
constexpr auto to_integer_impl(std::string_view str) {
	if (str.empty()) {
		throw std::invalid_argument("Cannot convert empty string to integer");
	}
	using policy = throw_policy<std::invalid_argument>;
	if constexpr(minimum < 0) {
		constexpr auto combined_max = normalize<std::max(
			bounded::detail::safe_abs(minimum),
			bounded::detail::safe_abs(maximum)
		)>;
		auto negative = false;
		if (str.front() == '-') {
			str.remove_prefix(1);
			if (str.empty()) {
				throw std::invalid_argument("Cannot convert empty string to integer");
			}
			negative = true;
		}
		auto const positive_result = bounded::detail::to_integer_positive_impl<combined_max>(str);
		return integer<minimum, maximum, policy>(BOUNDED_CONDITIONAL(negative, -positive_result, positive_result));
	} else {
		return integer<minimum, maximum, policy>(bounded::detail::to_integer_positive_impl<maximum>(str));
	}
}

} // namespace detail

// TODO: Support things other than throwing an exception?
template<auto minimum, auto maximum>
constexpr auto to_integer(std::string_view str) {
	return integer(detail::to_integer_impl<detail::normalize<minimum>, detail::normalize<maximum>>(str), bounded::null_policy{});
}

// This overload does not currently use the overflow policy of the Integer.
template<typename Integer>
constexpr auto to_integer(std::string_view const str) {
	return static_cast<Integer>(to_integer<detail::builtin_min_value<Integer>, detail::builtin_max_value<Integer>>(str));
}

} // namespace bounded

