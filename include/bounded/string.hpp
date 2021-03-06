// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/int128.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

#include <string>

namespace bounded {

// Import to_string for the numeric types
using std::to_string;

#if defined BOUNDED_DETAIL_HAS_128_BIT

// TODO: Make this more efficient
inline auto to_string(detail::uint128_t x) {
	if (x == 0) {
		return std::string({'0'});
	}
	auto result = std::string();
	while (x > 0) {
		result.push_back(static_cast<char>(x % 10 + '0'));
		x /= 10;
	}
	std::reverse(result.begin(), result.end());
	return result;
}

// TODO: Make this more efficient
inline auto to_string(detail::int128_t const x) {
	return x >= 0 ? to_string(static_cast<detail::uint128_t>(x)) : '-' + to_string(static_cast<detail::uint128_t>(x));
}

#endif

auto to_string(bounded_integer auto const x) {
	return to_string(+x.value());
}

} // namespace bounded

