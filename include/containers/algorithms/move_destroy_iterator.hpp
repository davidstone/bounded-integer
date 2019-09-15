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
	static_assert(std::is_nothrow_move_constructible_v<T>, "Please provide a noexcept move_destroy overload for your type or mark its move constructor noexcept.");
	static_assert(std::is_nothrow_destructible_v<T>, "Do not mark your destructor as noexcept(false)");
	auto result = std::move(ref);
	bounded::destroy(ref);
	return result;
}

template<typename Iterator>
constexpr auto move_destroy_iterator(Iterator it_) {
	return ::containers::transform_iterator_dereference(it_, [](Iterator const it) noexcept {
		if constexpr (std::is_reference_v<decltype(*it)>) {
			return move_destroy(*it);
		} else {
			return *it;
		}
	});
}

}	// namespace containers
