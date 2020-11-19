// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

struct default_dereference {
	static constexpr decltype(auto) dereference(iterator auto const & it) {
		return *it;
	}
};

struct default_add {
	static constexpr auto add(iterator auto it, auto const & offset) OPERATORS_RETURNS(
		std::move(it) + offset
	)
	static constexpr auto add(iterator auto it, bounded::constant_t<1>) {
		++it;
		return it;
	}
};

struct default_subtract {
	static constexpr auto subtract(iterator auto const & lhs, iterator auto const & rhs) OPERATORS_RETURNS(
		lhs - rhs
	)
};

struct default_compare {
	template<typename LHS, typename RHS> requires bounded::ordered<LHS, RHS>
	static constexpr auto compare(LHS const & lhs, RHS const & rhs) {
		return lhs <=> rhs;
	}
	template<typename LHS, typename RHS> requires bounded::equality_comparable<LHS, RHS>
	static constexpr auto equal(LHS const & lhs, RHS const & rhs) {
		return lhs == rhs;
	}
};

} // namespace containers
