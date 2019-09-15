// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>
#include <utility>

namespace containers {

template<typename Function>
struct scope_guard {
	constexpr explicit scope_guard(Function f):
		m_function(std::move(f)),
		m_is_active(true)
	{
	}
	
	constexpr scope_guard(scope_guard && other) noexcept(std::is_nothrow_move_constructible_v<Function>):
		m_function(std::move(other.m_function)),
		m_is_active(std::exchange(other.m_is_active, false))
	{
	}

	~scope_guard() {
		if (m_is_active) {
			std::move(m_function)();
		}
	}
	
	constexpr void dismiss() {
		m_is_active = false;
	}

private:
	Function m_function;
	bool m_is_active;
};

}	// namespace containers
