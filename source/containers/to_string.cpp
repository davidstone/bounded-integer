// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.to_string;

import containers.algorithms.reverse;

import containers.push_back;
import containers.string;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;
namespace containers {

constexpr auto to_string_impl(bounded::bounded_integer auto x, bool const include_minus) -> containers::string {
	auto result = containers::string();
	do {
		::containers::push_back(result, static_cast<char>(x % 10_bi + bounded::constant<'0'>));
		x /= 10_bi;
	} while (x > 0_bi);
	if (include_minus) {
		::containers::push_back(result, '-');
	}
	::containers::reverse(result);
	return result;
}

template<typename T>
constexpr auto include_zero(T const x) {
	return bounded::integer<0, bounded::builtin_max_value<T>>(x);
}

export constexpr auto to_string(bounded::bounded_integer auto const x) -> containers::string {
	return ::containers::to_string_impl(::containers::include_zero(bounded::abs(x)), x < 0_bi);
}

export constexpr auto to_string(bounded::builtin_integer auto const x) -> containers::string {
	return containers::to_string(bounded::integer(x));
}

} // namespace containers

static_assert(::containers::to_string(0_bi) == "0");
static_assert(::containers::to_string(1_bi) == "1");
static_assert(::containers::to_string(15_bi) == "15");
static_assert(::containers::to_string(-1_bi) == "-1");
static_assert(::containers::to_string(bounded::integer<-100, 100>(7_bi)) == "7");