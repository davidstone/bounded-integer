// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <string>

#include <numeric_traits/has_int128.hpp>

export module bounded.check_in_range;

import bounded.integer;
import bounded.comparison;
import bounded.comparison_builtin;
import bounded.is_bounded_integer;

import numeric_traits;
import std_module;

namespace bounded {

using std::to_string;

#if defined NUMERIC_TRAITS_HAS_INT128

auto to_string(numeric_traits::uint128_t x) {
	auto result = std::string();
	do {
		result.push_back(static_cast<char>(x % 10 + '0'));
		x /= 10;
	} while (x > 0);
	std::reverse(result.begin(), result.end());
	return result;
}

auto to_string(numeric_traits::int128_t const x) {
	return x >= 0 ? to_string(static_cast<numeric_traits::uint128_t>(x)) : '-' + to_string(-static_cast<numeric_traits::uint128_t>(x));
}

#endif

auto to_string(bounded_integer auto const x) {
	return to_string(+x.value());
}

export template<typename Exception = std::range_error>
constexpr auto check_in_range(integral auto const value, bounded_integer auto const minimum, bounded_integer auto const maximum) {
	if (minimum <= value and value <= maximum) {
		return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
	} else {
		throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
}

export template<typename Target, typename Exception = std::range_error>
constexpr auto check_in_range(integral auto const value) {
	return ::bounded::check_in_range<Exception>(value, numeric_traits::min_value<Target>, numeric_traits::max_value<Target>);
}

} // namespace bounded
