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

// This cannot use BOUNDED_NOEXCEPT_DECLTYPE because of the assert statement. On
// clang, it contains the predefined identifier __ASSERT_FUNCTION, which
// triggers an on-by-default warning: Wpredefined-identifier-outside-function.
//
// The alternative would be to write a new assert function that just gets
// __FILE__ and __LINE__, which may end up being a good idea if there are
// several functions that follow this pattern.

template<typename Iterator> requires iterator<Iterator>
constexpr auto operator_bracket(Iterator && iterator, index_type<std::decay_t<Iterator>> const index) BOUNDED_NOEXCEPT_DECLTYPE(
	*(BOUNDED_FORWARD(iterator) + index)
)

template<typename Range> requires range<Range>
constexpr auto operator_bracket(Range && range, index_type<std::decay_t<Range>> const index) noexcept(
	noexcept(index < size(range)) and
	noexcept(*(begin(BOUNDED_FORWARD(range)) + index)) and
	std::is_nothrow_move_constructible_v<decltype(*(begin(BOUNDED_FORWARD(range)) + index))>
) -> decltype(*(begin(BOUNDED_FORWARD(range)) + index)) {
	BOUNDED_ASSERT(index < size(range));
	return *(begin(BOUNDED_FORWARD(range)) + index);
}


}	// namespace detail
}	// namespace containers

#define CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(type) \
	template<typename Index> \
	constexpr auto operator[](Index const index) const & \
		noexcept(noexcept(::containers::detail::operator_bracket(std::declval<type const &>(), index))) -> \
		decltype(::containers::detail::operator_bracket(*this, index)) {\
		return ::containers::detail::operator_bracket(*this, index); \
	} \
	template<typename Index> \
	constexpr auto operator[](Index const index) & \
		noexcept(noexcept(::containers::detail::operator_bracket(std::declval<type &>(), index))) -> \
		decltype(::containers::detail::operator_bracket(*this, index)) {\
		return ::containers::detail::operator_bracket(*this, index); \
	} \
	template<typename Index> \
	constexpr auto operator[](Index const index) && \
		noexcept(noexcept(::containers::detail::operator_bracket(std::declval<type &&>(), index))) -> \
		decltype(::containers::detail::operator_bracket(std::move(*this), index)) {\
		return ::containers::detail::operator_bracket(std::move(*this), index); \
	}

