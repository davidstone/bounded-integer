// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform_iterator.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

// TODO: Integrate this with "relocate"
template<typename T>
constexpr auto move_destroy(T && ref) noexcept {
	static_assert(std::is_nothrow_move_constructible_v<T>, "Please provide a noexcept move_destroy overload for your type.");
	static_assert(std::is_nothrow_destructible_v<T>, "Do not mark your destructor as noexcept(false)");
	auto result = std::move(ref);
	bounded::destroy(ref);
	return result;
}

namespace detail {

struct move_destroy_dereference {
	template<typename Iterator>
	constexpr auto operator()(Iterator it) const noexcept(
		noexcept(*it) and std::is_nothrow_move_constructible_v<decltype(*it)>
	) {
		if constexpr (std::is_reference_v<decltype(*it)>) {
			return move_destroy(*it);
		} else {
			return *it;
		}
	}
};

}	// namespace detail

template<typename Iterator>
constexpr auto move_destroy_iterator(Iterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::transform_iterator_dereference(it, detail::move_destroy_dereference{})
)

}	// namespace containers
