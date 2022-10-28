// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

#include <numeric_traits/int128.hpp>

namespace bounded {
namespace detail {

inline constexpr auto right_shift = [](auto const lhs, auto const rhs) {
	return lhs >> rhs;
};

} // namespace detail

constexpr auto operator>>(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return detail::operator_overload(lhs_, rhs_, detail::right_shift, [](auto const lhs, auto const rhs) {
		// TODO: Broaden range
		return detail::min_max{
			static_cast<numeric_traits::max_signed_t>(lhs.min) >> rhs.max.value(),
			static_cast<numeric_traits::max_signed_t>(lhs.max) >> rhs.min.value()
		};
	});
}

}	// namespace bounded

