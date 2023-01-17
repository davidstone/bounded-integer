// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.comparison_function_object;

import bounded.comparison;

import std_module;

namespace bounded {
namespace detail {

template<typename Bound>
struct unary_equal_to {
	constexpr explicit unary_equal_to(Bound && bound):
		m_bound(OPERATORS_FORWARD(bound))
	{
	}
	constexpr auto operator()(auto const & other) const {
		return m_bound == other;
	}
private:
	Bound m_bound;
};

} // namespace detail

export constexpr auto equal_to() {
	return std::equal_to();
}
export template<typename T>
constexpr auto equal_to(T && t) {
	return detail::unary_equal_to<T>(OPERATORS_FORWARD(t));
}

} // namespace bounded
