// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.value_to_function;

import std_module;

namespace bounded {

export template<typename T>
struct value_to_function {
	static_assert(std::is_reference_v<T>);

	constexpr explicit value_to_function(T && ref) noexcept:
		m_ptr(std::addressof(ref))
	{
	}

	constexpr auto operator()() const & noexcept -> T & {
		return *m_ptr;
	}
	constexpr auto operator()() const && noexcept -> T {
		return static_cast<T>(*m_ptr);
	}

private:
	std::remove_reference_t<T> * m_ptr;
};

template<typename T>
value_to_function(T &&) -> value_to_function<T &&>;

} // namespace bounded