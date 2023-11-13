// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.transform_traits;

import containers.default_adapt_traits;
import containers.is_iterator;
import containers.iter_reference_t;

import std_module;

namespace containers {

export template<typename UnaryFunction>
struct transform_traits : default_add, default_subtract, default_compare {
	constexpr explicit transform_traits(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	template<iterator It> requires std::invocable<UnaryFunction const &, iter_reference_t<It>>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		return std::invoke(m_dereference, *it);
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

export template<typename UnaryFunction>
struct transform_traits_dereference : default_add, default_subtract, default_compare {
	constexpr explicit transform_traits_dereference(UnaryFunction dereference_):
		m_dereference(std::move(dereference_))
	{
	}
	
	template<iterator It> requires std::invocable<UnaryFunction const &, It>
	constexpr auto dereference(It const & it) const -> decltype(auto) {
		return std::invoke(m_dereference, it);
	}
private:
	[[no_unique_address]] UnaryFunction m_dereference;
};

} // namespace containers
