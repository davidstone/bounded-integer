// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/minmax.hpp>

// I would prefer not to implement this, as bounded::integer is not a collection
// of bits. However, many people use x & (y - 1) when they mean x % y, when y is
// a power of two. For instance, gcc's implementation of std::sort

namespace bounded {
namespace detail {

// #include <functional> is really expensive, and we just need this one part
inline constexpr auto bit_and = [](auto const lhs, auto const rhs) {
	return lhs & rhs;
};

} // namespace detail

constexpr auto operator&(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	// Assume always positive integers for now
	// Not the tightest possible bounds, but probably good enough for now
	return ::bounded::detail::operator_overload(lhs_, rhs_, detail::bit_and, [](auto const lhs, auto const rhs) {
		return detail::min_max{
			0,
			min(lhs.max, rhs.max).value()
		};
	});
}

}	// namespace bounded

