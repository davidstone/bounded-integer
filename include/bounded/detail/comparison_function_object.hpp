// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward.hpp>

#include <utility>

namespace bounded {
namespace detail {

struct compare_to_t {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const {
		return compare(lhs, rhs);
	}
};

struct binary_equal_to {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const {
		return lhs == rhs;
	}
};
template<typename Bound>
struct unary_equal_to {
private:
	// This has to be first to work around gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66256
	Bound m_bound;
public:
	constexpr explicit unary_equal_to(Bound && bound):
		m_bound(BOUNDED_FORWARD(bound))
	{
	}
	template<typename Other>
	constexpr auto operator()(Other const & other) const {
		return m_bound == other;
	}
};

// TODO: Add total ordering for pointer types
struct less_t {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const {
		return lhs < rhs;
	}
};
struct greater_t {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const {
		return lhs > rhs;
	}
};

}	// namespace detail

constexpr auto compare_3way() {
	return detail::compare_to_t{};
}

constexpr auto equal_to() {
	return detail::binary_equal_to{};
}
template<typename T>
constexpr auto equal_to(T && t) {
	return detail::unary_equal_to<T>(BOUNDED_FORWARD(t));
}

constexpr auto less() {
	return detail::less_t{};
}

constexpr auto greater() {
	return detail::greater_t{};
}

}	// namespace bounded
