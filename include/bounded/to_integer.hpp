// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/conditional.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/check_in_range.hpp>

#include <algorithm>
#include <stdexcept>
#include <string_view>

namespace bounded {
namespace detail {

template<auto maximum>
constexpr auto to_integer_positive_impl(std::string_view str) {
	constexpr auto check = [](auto const value, auto const max) {
		return check_in_range<std::invalid_argument>(value, constant<0>, max);
	};
	auto positive_result = integer<0, normalize<maximum>>(constant<0>);
	for (auto const c : str) {
		auto const shifted = positive_result * constant<10>;
		auto const digit = check(integer(c - '0'), constant<9>);
		positive_result = check(shifted + digit, constant<maximum>);
	}
	return positive_result;
}

} // namespace detail

// TODO: Support things other than throwing an exception?
template<auto minimum, auto maximum>
constexpr auto to_integer(std::string_view str) {
	if (str.empty()) {
		throw std::invalid_argument("Cannot convert empty string to integer");
	}

	auto impl = [&] {
		if constexpr(minimum < 0) {
			constexpr auto combined_max = std::max(
				bounded::detail::safe_abs(minimum),
				bounded::detail::safe_abs(maximum)
			);
			auto negative = false;
			if (str.front() == '-') {
				str.remove_prefix(1);
				if (str.empty()) {
					throw std::invalid_argument("Cannot convert empty string to integer");
				}
				negative = true;
			}
			auto const positive_result = bounded::detail::to_integer_positive_impl<combined_max>(str);
			return BOUNDED_CONDITIONAL(negative, -positive_result, positive_result);
		} else {
			return bounded::detail::to_integer_positive_impl<maximum>(str);
		}
	};
	return check_in_range<std::invalid_argument>(impl(), constant<minimum>, constant<maximum>);
}

template<typename Integer>
constexpr auto to_integer(std::string_view const str) {
	return static_cast<Integer>(to_integer<detail::builtin_min_value<Integer>, detail::builtin_max_value<Integer>>(str));
}

} // namespace bounded
