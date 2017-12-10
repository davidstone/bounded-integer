// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Predicate>
struct negate_t {
	constexpr negate_t(Predicate && predicate) noexcept(std::is_nothrow_move_constructible<Predicate>::value):
		m_predicate(std::move(predicate))
	{
	}
	template<typename... Args>
	constexpr decltype(auto) operator()(Args && ... args) const BOUNDED_NOEXCEPT(
		!m_predicate(std::forward<Args>(args)...)
	)
private:
	Predicate m_predicate;
};

}	// namespace detail

template<typename Predicate>
constexpr auto negate(Predicate predicate) BOUNDED_NOEXCEPT(
	detail::negate_t<Predicate>(std::move(predicate))
)

}	// namespace containers
