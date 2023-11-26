// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/conditional.hpp>

export module bounded.to_integer;

import bounded.arithmetic.operators;
import bounded.arithmetic.safe_abs;
import bounded.builtin_min_max_value;
import bounded.check_in_range;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.literal;
import bounded.normalize;

import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace bounded {

constexpr auto check(auto const value, auto const max) {
	return check_in_range<std::invalid_argument>(value, 0_bi, max);
};

constexpr auto as_digit(char const c) -> integer<0, 9> {
	return check(c - '0', 9_bi);
}

template<auto maximum>
constexpr auto to_integer_positive_impl(std::string_view const str) {
	auto positive_result = integer<0, normalize<maximum>>(0_bi);
	for (auto const c : str) {
		auto const shifted = positive_result * 10_bi;
		positive_result = check(shifted + as_digit(c), constant<maximum>);
	}
	return positive_result;
}

// TODO: Support things other than throwing an exception?
export template<auto minimum, auto maximum>
constexpr auto to_integer(std::string_view str) {
	if (str.empty()) {
		throw std::invalid_argument("Cannot convert empty string to integer");
	}

	auto impl = [&] {
		if constexpr(minimum < 0) {
			constexpr auto combined_max = std::max(
				safe_abs(minimum),
				safe_abs(maximum)
			);
			auto negative = false;
			if (str.front() == '-') {
				str.remove_prefix(1);
				if (str.empty()) {
					throw std::invalid_argument("Cannot convert empty string to integer");
				}
				negative = true;
			}
			auto const positive_result = ::bounded::to_integer_positive_impl<combined_max>(str);
			return BOUNDED_CONDITIONAL(negative, -positive_result, positive_result);
		} else {
			return ::bounded::to_integer_positive_impl<maximum>(str);
		}
	};
	return check_in_range<std::invalid_argument>(impl(), constant<minimum>, constant<maximum>);
}

export template<typename Integer>
constexpr auto to_integer(std::string_view const str) {
	return static_cast<Integer>(to_integer<builtin_min_value<Integer>, builtin_max_value<Integer>>(str));
}

} // namespace bounded

namespace {

static_assert(bounded::to_integer<0, 0>("0") == 0_bi);
static_assert(bounded::to_integer<-100, 100>("0") == 0_bi);
static_assert(bounded::to_integer<-100, 100>("1") == 1_bi);
static_assert(bounded::to_integer<-100, 100>("-1") == -1_bi);
static_assert(bounded::to_integer<0, 100>("50") == 50_bi);
static_assert(bounded::to_integer<-100, -4>("-60") == -60_bi);
static_assert(bounded::to_integer<99, 100>("100") == 100_bi);
static_assert(
	bounded::to_integer<
		static_cast<std::int64_t>(numeric_traits::min_value<std::int32_t>) - 1,
		static_cast<std::uint64_t>(numeric_traits::max_value<std::uint32_t>) + 1
	>("4294967296") == 4294967296_bi
);
static_assert(
	bounded::to_integer<
		static_cast<std::int64_t>(numeric_traits::min_value<std::int32_t>) - 1,
		static_cast<std::uint64_t>(numeric_traits::max_value<std::uint32_t>) + 1
	>("-2147483648") == -2147483648_bi
);

static_assert(homogeneous_equals(
	bounded::to_integer<156, 325>("256"),
	bounded::to_integer<bounded::integer<156, 325>>("256")
));

} // namespace
