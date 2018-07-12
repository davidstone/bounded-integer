// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/noexcept.hpp>

#include <utility>

namespace bounded {
namespace detail {

struct binary_equal_to {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const BOUNDED_NOEXCEPT(
		compare(lhs, rhs) == 0
	)
};
template<typename Bound>
struct unary_equal_to {
private:
	// This has to be first to work around gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66256
	Bound m_bound;
public:
	constexpr explicit unary_equal_to(Bound && bound) noexcept(std::is_nothrow_constructible_v<Bound, Bound &&>):
		m_bound(std::forward<Bound>(bound))
	{
	}
	template<typename Other>
	constexpr auto operator()(Other const & other) const BOUNDED_NOEXCEPT(
		compare(m_bound, other) == 0
	)
};

// TODO: Add total ordering for pointer types
struct less_t {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const BOUNDED_NOEXCEPT(
		compare(lhs, rhs) < 0
	)
};
struct greater_t {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS const & lhs, RHS const & rhs) const BOUNDED_NOEXCEPT(
		compare(lhs, rhs) > 0
	)
};

}	// namespace detail

constexpr auto equal_to() noexcept {
	return detail::binary_equal_to{};
}
template<typename T>
constexpr auto equal_to(T && t) BOUNDED_NOEXCEPT(
	detail::unary_equal_to<T>(std::forward<T>(t))
)

constexpr auto less() noexcept {
	return detail::less_t{};
}

constexpr auto greater() noexcept {
	return detail::greater_t{};
}

}	// namespace bounded
