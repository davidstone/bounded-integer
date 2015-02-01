// Reduces boilerplate for simple policies
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef BOUNDED_INTEGER_POLICY_BASIC_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_BASIC_POLICY_HPP_

#include "is_overflow_policy.hpp"
#include "../enable_if.hpp"
#include <type_traits>

namespace bounded {

template<typename policy_type>
struct basic_policy : private policy_type {
	static_assert(
		std::is_empty<policy_type>::value,
		"basic_policy can only be used with stateless policies"
	);
	using overflow_policy_tag = void;

	// constexpr and noexcept are implied by = default
	basic_policy(basic_policy const &) = default;
	basic_policy(basic_policy &&) = default;

	constexpr basic_policy() noexcept {}
	template<typename T, enable_if_t<std::is_same<std::decay_t<T>, basic_policy>::value> = clang_dummy>
	constexpr basic_policy(T &&) noexcept {
	}
	template<typename T, enable_if_t<
		is_overflow_policy<T>::value and
		!std::is_same<std::decay_t<T>, basic_policy>::value
	> = clang_dummy>
	constexpr explicit basic_policy(T &&) noexcept {
	}

	auto operator=(basic_policy const &) noexcept -> basic_policy & = default;
	auto operator=(basic_policy &&) noexcept -> basic_policy & = default;
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto operator=(T &&) noexcept -> basic_policy & {
		return *this;
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	auto operator=(T &&) volatile noexcept -> basic_policy volatile & {
		return *this;
	}

	using policy_type::assignment;
	
	using policy_type::is_modulo;
	using policy_type::overflow_is_error;
};

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_BASIC_POLICY_HPP_
