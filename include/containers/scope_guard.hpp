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
struct scope_guard_t : private std::tuple<Function> {
	template<typename F>
	constexpr explicit scope_guard_t(F && function) BOUNDED_NOEXCEPT_INITIALIZATION(
		std::tuple<Function>(std::forward<F>(function))
	) {
	}
	
	constexpr scope_guard_t(scope_guard_t && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		std::tuple<Function>(static_cast<std::tuple<Function> &&>(other))
	) {
		m_active = std::exchange(other.m_active, false);
	}

	~scope_guard_t() {
		if (m_active) {
			std::get<Function>(*this)();
		}
	}
	
	constexpr void dismiss() noexcept {
		m_active = false;
	}

private:
	bool m_active;
};

}	// namespace detail

template<typename Function>
constexpr auto scope_guard(Function && function) BOUNDED_NOEXCEPT_VALUE(
	detail::scope_guard_t<std::decay_t<Function>>(std::forward<Function>(function))
)

}	// namespace containers
