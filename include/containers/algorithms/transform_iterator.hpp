// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_adapter.hpp>

namespace containers {
namespace detail {

template<typename UnaryFunction>
struct transform_traits : default_add, default_subtract, default_compare {
private:
	UnaryFunction m_dereference;

public:
	constexpr explicit transform_traits(UnaryFunction && dereference):
		m_dereference(BOUNDED_FORWARD(dereference))
	{
	}
	
	template<typename Iterator>
	constexpr auto dereference(Iterator const & it) const BOUNDED_NOEXCEPT_DECLTYPE(
		m_dereference(it)
	)
};

}	// namespace detail

template<typename Iterator, typename UnaryFunction>
constexpr auto transform_iterator(Iterator it, UnaryFunction && dereference) {
	return adapt_iterator(std::move(it), detail::transform_traits<UnaryFunction>(BOUNDED_FORWARD(dereference)));
}

}	// namespace containers
