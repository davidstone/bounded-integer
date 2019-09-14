// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {
	
constexpr auto compare(bounded_integer auto const lhs, detail_builtin_integer auto const rhs) noexcept {
	return compare(lhs, integer(rhs));
}
constexpr auto compare(detail_builtin_integer auto const lhs, bounded_integer auto const rhs) noexcept {
	return compare(integer(lhs), rhs);
}

constexpr auto operator==(bounded_integer auto const lhs, detail_builtin_integer auto const rhs) noexcept {
	return lhs == integer(rhs);
}
constexpr auto operator==(detail_builtin_integer auto const lhs, bounded_integer auto const rhs) noexcept {
	return integer(lhs) == rhs;
}

constexpr auto compare(bounded_integer auto const lhs, bool const rhs) = delete;
constexpr auto compare(bool const lhs, bounded_integer auto const rhs) = delete;

constexpr auto operator==(bounded_integer auto const lhs, bool const rhs) = delete;
constexpr auto operator==(bool const lhs, bounded_integer auto const rhs) = delete;

}	// namespace bounded
