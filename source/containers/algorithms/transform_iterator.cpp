// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/returns.hpp>

export module containers.algorithms.transform_iterator;

import containers.default_adapt_traits;
import containers.is_iterator;
import containers.iterator_adapter;

import std_module;

namespace containers {

export template<typename UnaryFunction>
struct transform_traits : default_add, default_subtract, default_compare {
private:
	[[no_unique_address]] UnaryFunction m_dereference;

public:
	constexpr explicit transform_traits(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	constexpr auto dereference(iterator auto const & it) const OPERATORS_RETURNS(
		std::invoke(m_dereference, *it)
	)
};

export template<typename UnaryFunction>
struct transform_traits_dereference : default_add, default_subtract, default_compare {
private:
	[[no_unique_address]] UnaryFunction m_dereference;

public:
	constexpr explicit transform_traits_dereference(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	constexpr auto dereference(iterator auto const & it) const OPERATORS_RETURNS(
		std::invoke(m_dereference, it)
	)

};

export constexpr auto transform_iterator(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), transform_traits(std::move(dereference)));
}

export constexpr auto transform_iterator_dereference(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), transform_traits_dereference(std::move(dereference)));
}

} // namespace containers
