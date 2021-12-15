// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/construct_destroy.hpp>

#include <type_traits>

namespace bounded_relocate_adl_detail {

constexpr auto relocate(auto & ref) noexcept {
	using T = std::remove_reference_t<decltype(ref)>;
	static_assert(std::is_nothrow_move_constructible_v<T>, "Please provide a noexcept relocate overload for your type or mark its move constructor noexcept.");
	static_assert(std::is_nothrow_destructible_v<T>, "Do not mark your destructor as noexcept(false)");
	auto result = std::move(ref);
	bounded::destroy(ref);
	return result;
}

constexpr auto relocate_impl(auto & ref) noexcept {
	return relocate(ref);
}

} // namespace relocate_adl_detail
namespace bounded {

inline constexpr auto relocate = [](detail::non_const auto & ref) noexcept {
	return ::bounded_relocate_adl_detail::relocate_impl(ref);
};

inline constexpr auto relocate_at = [](detail::non_const auto & destination, detail::non_const auto & source) noexcept -> auto & {
	return bounded::construct(destination, [&] { return bounded::relocate(source); });
};

} // namespace bounded
