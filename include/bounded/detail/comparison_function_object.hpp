// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/noexcept.hpp>

#include <utility>

namespace bounded {
namespace detail {

struct binary_equal_to {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) == std::forward<RHS>(rhs)
	)
};
template<typename Bound>
struct unary_equal_to {
private:
	// This has to be first to work around gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66256
	Bound m_bound;
public:
	constexpr explicit unary_equal_to(Bound && bound) noexcept(std::is_nothrow_constructible<Bound, Bound &&>{}):
		m_bound(std::forward<Bound>(bound))
	{
	}
	template<typename Other>
	constexpr decltype(auto) operator()(Other && other) const BOUNDED_NOEXCEPT(
		m_bound == std::forward<Other>(other)
	)
};

// TODO: Add total ordering for pointer types
struct less_t {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) < std::forward<RHS>(rhs)
	)
};
struct greater_t {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) > std::forward<RHS>(rhs)
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
