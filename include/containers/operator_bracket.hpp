// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/assert.hpp>

#include <type_traits>

// We currently cannot have a free-function `operator[]`. However, all sequence
// containers have the same implementation of it. This hides the boilerplate.

namespace containers {
namespace detail {

template<typename Iterator>
concept forward_random_access_iterator = iterator<Iterator> and iterator<decltype(std::declval<Iterator>() + std::declval<index_type<std::decay_t<Iterator>>>())>;

template<typename Range>
concept forward_random_access_range = range<Range> and forward_random_access_iterator<decltype(begin(std::declval<Range>()))>;

template<forward_random_access_iterator Iterator>
constexpr decltype(auto) operator_bracket(Iterator && iterator, index_type<std::decay_t<Iterator>> const index) {
	return *(BOUNDED_FORWARD(iterator) + index);
}

template<range Range>
constexpr decltype(auto) operator_bracket(Range && range, index_type<std::decay_t<Range>> const index) {
	BOUNDED_ASSERT(index < size(range));
	return *(begin(BOUNDED_FORWARD(range)) + index);
}


}	// namespace detail
}	// namespace containers

#define CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(type) \
	template<typename Index> \
	constexpr auto operator[](Index const index) const & -> \
		decltype(::containers::detail::operator_bracket(*this, index)) {\
		return ::containers::detail::operator_bracket(*this, index); \
	} \
	template<typename Index> \
	constexpr auto operator[](Index const index) & -> \
		decltype(::containers::detail::operator_bracket(*this, index)) {\
		return ::containers::detail::operator_bracket(*this, index); \
	} \
	template<typename Index> \
	constexpr auto operator[](Index const index) && -> \
		decltype(::containers::detail::operator_bracket(std::move(*this), index)) {\
		return ::containers::detail::operator_bracket(std::move(*this), index); \
	}

