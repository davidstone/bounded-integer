// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <tuple>
#include <utility>

namespace containers {
namespace detail {

template<typename Function>
struct scope_guard_t {
	template<typename F>
	constexpr explicit scope_guard_t(F && function) noexcept(std::is_nothrow_constructible<Function, Function &&>::value):
		m_data(std::forward<F>(function), true)
	{
	}
	
	constexpr scope_guard_t(scope_guard_t && other) noexcept(std::is_nothrow_move_constructible<Function>::value):
		m_data(std::move(other.m_data))
	{
	}

	~scope_guard_t() {
		if (std::get<bool>(m_data)) {
			std::get<Function>(m_data)();
		}
	}
	
	constexpr void dismiss() noexcept {
		std::get<bool>(m_data) = false;
	}

private:
	std::tuple<Function, bool> m_data;
};

}	// namespace detail

template<typename Function>
constexpr auto scope_guard(Function && function) BOUNDED_NOEXCEPT_VALUE(
	detail::scope_guard_t<std::decay_t<Function>>(std::forward<Function>(function))
)

}	// namespace containers
