// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.scope_fail;

import bounded.scope_guard;

import std_module;

namespace bounded {

export template<typename Function>
struct scope_fail {
	constexpr explicit scope_fail(Function f):
		m_uncaught_exceptions(std::uncaught_exceptions()),
		m_scope_guard(std::move(f))
	{
	}

	constexpr scope_fail(scope_fail && other) = default;

	constexpr ~scope_fail() {
		if (m_uncaught_exceptions == std::uncaught_exceptions()) {
			dismiss();
		}
	}
	
	constexpr void dismiss() {
		m_scope_guard.dismiss();
	}

private:
	int m_uncaught_exceptions;
	scope_guard<Function> m_scope_guard;
};

} // namespace bounded
