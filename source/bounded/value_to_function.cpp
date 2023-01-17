// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.value_to_function;

import std_module;

namespace bounded {

export template<typename T>
struct value_to_function {
	constexpr explicit value_to_function(T && ref):
		m_ptr(std::addressof(ref))
	{
	}

	constexpr T & operator()() const & {
		return *m_ptr;
	}
	constexpr T && operator()() && {
		return static_cast<T &&>(*m_ptr);
	}

private:
	std::remove_reference_t<T> * m_ptr;
};

template<typename T>
value_to_function(T &&) -> value_to_function<T &&>;

} // namespace bounded