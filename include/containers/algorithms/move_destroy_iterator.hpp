// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_adapter.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

constexpr struct {
	template<typename T>
	constexpr auto operator()(T && ref) const noexcept(std::is_nothrow_move_constructible<std::decay_t<T>>{}) {
		auto result = std::move(ref);
		static_assert(noexcept(bounded::destroy(ref)), "Do not mark your destructor as noexcept(false)");
		bounded::destroy(ref);
		return result;
	}
} move_destroy;

struct move_destroy_dereference {
	template<typename Iterator, BOUNDED_REQUIRES(std::is_reference<decltype(*std::declval<Iterator>())>{})>
	constexpr auto operator()(Iterator && it) const BOUNDED_NOEXCEPT_VALUE(
		move_destroy(*std::forward<Iterator>(it))
	)
};

}	// namespace detail

template<typename Iterator>
constexpr auto move_destroy_iterator(Iterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::move_destroy_dereference{})
)

template<typename Iterator>
using move_destroy_iterator_t = decltype(move_destroy_iterator(std::declval<Iterator>()));

}	// namespace containers
