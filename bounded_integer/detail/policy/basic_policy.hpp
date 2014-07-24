// Reduces boilerplate for simple policies
// Copyright (C) 2014 David Stone
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
#include <cstdint>
#include <utility>

namespace bounded {

template<typename policy_type>
class basic_policy {
public:
	using overflow_policy_tag = void;
	constexpr basic_policy() noexcept {}
	constexpr basic_policy(basic_policy const &) noexcept = default;
	constexpr basic_policy(basic_policy &&) noexcept = default;
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

	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum)
		noexcept(noexcept(policy_type{}.assignment(std::forward<T>(value), std::forward<Minimum>(minimum), std::forward<Maximum>(maximum)))) {
		return policy_type{}.assignment(std::forward<T>(value), std::forward<Minimum>(minimum), std::forward<Maximum>(maximum));
	}
	
	static constexpr bool is_modulo = policy_type::is_modulo;
	static constexpr bool overflow_is_error = policy_type::overflow_is_error;
};

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_BASIC_POLICY_HPP_
