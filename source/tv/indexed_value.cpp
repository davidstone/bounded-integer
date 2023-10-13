// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.indexed_value;

import bounded;
import std_module;

namespace tv {

export template<typename T, std::size_t n>
struct indexed_value {
	static_assert(std::is_reference_v<T>);
	constexpr explicit indexed_value(T value_):
		m_value(OPERATORS_FORWARD(value_))
	{
	}
	constexpr auto value() const & -> auto & {
		return m_value;
	}
	constexpr auto value() && -> auto && {
		return OPERATORS_FORWARD(m_value);
	}
	static constexpr auto index = bounded::constant<n>;

private:
	T m_value;
};

} // namespace tv
