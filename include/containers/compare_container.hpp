// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>

namespace containers {
namespace range_equality {

struct base {
	template<range T>
	friend constexpr auto operator==(T const & lhs, T const & rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}
};

} // namespace range_equality

namespace lexicographical_comparison {

struct base {
	template<range T>
	friend constexpr auto operator<=>(T const & lhs, T const & rhs) {
		return ::containers::lexicographical_compare_3way(lhs, rhs);
	}
	template<range T>
	friend constexpr auto operator==(T const & lhs, T const & rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}
};

} // namespace lexicographical_comparison

namespace shortlex_comparison {

struct base {
	template<range T>
	friend constexpr auto operator<=>(T const & lhs, T const & rhs) {
		return ::containers::shortlex_compare(lhs, rhs);
	}
	template<range T>
	friend constexpr auto operator==(T const & lhs, T const & rhs) -> bool {
		return ::containers::equal(lhs, rhs);
	}
};

} // namespace shortlex_comparison
} // namespace containers
