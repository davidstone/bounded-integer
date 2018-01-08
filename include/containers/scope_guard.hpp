// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/tuple.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
using namespace bounded::literal;

template<typename Function>
struct scope_guard_t {
	template<typename F>
	constexpr explicit scope_guard_t(F && f) noexcept(std::is_nothrow_constructible<Function, F &&>::value):
		m_data(std::forward<F>(f), true)
	{
	}
	
	constexpr scope_guard_t(scope_guard_t && other) noexcept(std::is_nothrow_move_constructible<Function>::value):
		m_data(
			std::move(other.m_data[function]),
			std::exchange(other.m_data[is_active], false)
		)
	{
	}

	~scope_guard_t() {
		// TODO: linker error if I use function and is_active here
		if (m_data[1_bi]) {
			std::move(m_data)[0_bi]();
		}
	}
	
	constexpr void dismiss() noexcept {
		m_data[is_active] = false;
	}

private:
	static constexpr auto function = 0_bi;
	static constexpr auto is_active = 1_bi;
	tuple<Function, bool> m_data;
};

}	// namespace detail

template<typename Function>
constexpr auto scope_guard(Function && function) BOUNDED_NOEXCEPT_VALUE(
	detail::scope_guard_t<std::decay_t<Function>>(std::forward<Function>(function))
)

}	// namespace containers
