// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/construct_destroy.hpp>
#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

constexpr auto relocate(auto & ref) noexcept {
	using T = std::remove_reference_t<decltype(ref)>;
	static_assert(std::is_nothrow_move_constructible_v<T>, "Please provide a noexcept relocate overload for your type or mark its move constructor noexcept.");
	static_assert(std::is_nothrow_destructible_v<T>, "Do not mark your destructor as noexcept(false)");
	auto result = std::move(ref);
	bounded::destroy(ref);
	return result;
}

} // namespace containers
