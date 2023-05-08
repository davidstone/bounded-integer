// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.to_any_string;

import containers.algorithms.reverse;

import containers.push_back;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;
namespace containers {

template<typename Result>
constexpr auto to_any_string_impl(bounded::bounded_integer auto x, bool const include_minus) -> Result {
	auto result = Result();
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

export template<typename Result>
constexpr auto to_any_string(bounded::bounded_integer auto const x) -> Result {
	return ::containers::to_any_string_impl<Result>(::containers::include_zero(bounded::abs(x)), x < 0_bi);
}

export template<typename Result>
constexpr auto to_any_string(bounded::builtin_integer auto const x) -> Result {
	return containers::to_any_string<Result>(bounded::integer(x));
}

} // namespace containers