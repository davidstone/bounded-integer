// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.relocate;

import bounded.construct_at;
import bounded.destroy;
import bounded.non_const;

import std_module;

namespace bounded_relocate_adl_detail {

template<typename T>
constexpr auto relocate(T & ref) noexcept {
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

export constexpr auto relocate = [](non_const auto & ref) noexcept {
	return ::bounded_relocate_adl_detail::relocate_impl(ref);
};

export constexpr auto relocate_at = [](non_const auto & destination, non_const auto & source) noexcept -> auto & {
	return bounded::construct_at(destination, [&] { return bounded::relocate(source); });
};

} // namespace bounded
