// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_adapter.hpp>

namespace containers {
namespace detail {

// TODO: [[no_unique_address]]
template<typename UnaryFunction>
struct transform_traits : default_add, default_subtract, default_compare, private UnaryFunction {
	transform_traits() = default;
	constexpr explicit transform_traits(UnaryFunction dereference):
		UnaryFunction(std::move(dereference))
	{
	}
	
	template<typename Iterator>
	constexpr auto dereference(Iterator const it) const
		noexcept(noexcept(std::declval<UnaryFunction const &>()(*it)))
		-> decltype(std::declval<UnaryFunction const &>()(*it))
	{
		return static_cast<UnaryFunction const &>(*this)(*it);
	}
};

template<typename UnaryFunction>
struct transform_traits_dereference : default_add, default_subtract, default_compare, private UnaryFunction {
	constexpr explicit transform_traits_dereference(UnaryFunction dereference):
		UnaryFunction(std::move(dereference))
	{
	}
	
	template<typename Iterator>
	constexpr auto dereference(Iterator const it) const
		noexcept(noexcept(std::declval<UnaryFunction const &>()(it)))
		-> decltype(std::declval<UnaryFunction const &>()(it))
	{
		return static_cast<UnaryFunction const &>(*this)(it);
	}
};

}	// namespace detail

template<typename Iterator, typename UnaryFunction>
constexpr auto transform_iterator(Iterator it, UnaryFunction dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits(std::move(dereference)));
}

template<typename Iterator, typename UnaryFunction>
constexpr auto transform_iterator_dereference(Iterator it, UnaryFunction dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits_dereference(std::move(dereference)));
}

}	// namespace containers
