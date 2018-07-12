// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

#include <type_traits>
#include <utility>

namespace containers {
using namespace bounded::literal;

template<typename Function>
struct scope_guard {
	constexpr explicit scope_guard(Function f) noexcept(std::is_nothrow_move_constructible_v<Function>):
		m_data(std::move(f), true)
	{
	}
	
	constexpr scope_guard(scope_guard && other) noexcept(std::is_nothrow_move_constructible_v<Function>):
		m_data(
			std::move(other.m_data[function]),
			std::exchange(other.m_data[is_active], false)
		)
	{
	}

	~scope_guard() {
		if (m_data[is_active]) {
			std::move(m_data)[function]();
		}
	}
	
	constexpr void dismiss() noexcept {
		m_data[is_active] = false;
	}

private:
	static inline constexpr auto function = 0_bi;
	static inline constexpr auto is_active = 1_bi;
	tuple<Function, bool> m_data;
};

}	// namespace containers
