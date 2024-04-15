// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.value_wrapper;

import std_module;

namespace containers {

export template<typename T>
struct value_wrapper {
	using type = T;
	value_wrapper() = default;
	constexpr value_wrapper(T value):
		m_value(std::move(value))
	{
	}
	constexpr auto get() const & -> T const & {
		return m_value;
	}
	constexpr auto get() && -> T && {
		return std::move(m_value);
	}
private:
	[[no_unique_address]] T m_value;
};

} // namespace containers
