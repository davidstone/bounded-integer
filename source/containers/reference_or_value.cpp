// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.reference_or_value;

import bounded;
import std_module;

namespace containers {

template<typename T>
struct value_wrapper {
	using type = T;
	constexpr value_wrapper(T value):
		m_value(value)
	{
	}
	constexpr auto get() const -> T const & {
		return m_value;
	}
private:
	[[no_unique_address]] T m_value;
};

template<typename T>
constexpr auto can_store_copy =
	bounded::mostly_trivial<T> and
	sizeof(T) <= sizeof(std::reference_wrapper<T>);

export template<typename T>
using reference_or_value = std::conditional_t<
	can_store_copy<T>,
	value_wrapper<std::remove_const_t<T>>,
	std::reference_wrapper<T const>
>;

template<typename T>
constexpr auto is_wrapped = false;

template<typename T>
constexpr auto is_wrapped<value_wrapper<T>> = true;

template<typename T>
constexpr auto is_wrapped<std::reference_wrapper<T>> = true;

export template<typename T>
constexpr auto unwrap(T && reference) -> auto && {
	if constexpr (is_wrapped<std::remove_cvref_t<T>>) {
		return reference.get();
	} else {
		return OPERATORS_FORWARD(reference);
	}
}

} // namespace containers
