// Also known as saturation
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

#ifndef BOUNDED_INTEGER_POLICY_CLAMP_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_CLAMP_POLICY_HPP_

#include "is_overflow_policy.hpp"
#include "../enable_if.hpp"
#include <cstdint>

namespace bounded_integer {

class clamp_policy {
public:
	using overflow_policy_tag = void;
	constexpr clamp_policy() noexcept {}
	constexpr clamp_policy(clamp_policy const &) noexcept = default;
	constexpr clamp_policy(clamp_policy &&) noexcept = default;
	template<typename T, enable_if_t<std::is_same<typename std::decay<T>::type, clamp_policy>::value> = clang_dummy>
	constexpr clamp_policy(T &&) noexcept {
	}
	template<typename T, enable_if_t<
		is_overflow_policy<T>::value and
		!std::is_same<typename std::decay<T>::type, clamp_policy>::value
	> = clang_dummy>
	constexpr explicit clamp_policy(T &&) noexcept {
	}

	clamp_policy & operator=(clamp_policy const &) noexcept = default;
	clamp_policy & operator=(clamp_policy &&) noexcept = default;
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	clamp_policy & operator=(T &&) noexcept {
		return *this;
	}
	template<typename T, enable_if_t<is_overflow_policy<T>::value> = clang_dummy>
	clamp_policy volatile & operator=(T &&) volatile noexcept {
		return *this;
	}

	template<typename T, typename Minimum, typename Maximum>
	static constexpr intmax_t assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept {
		return
			(value <= minimum) ? static_cast<intmax_t>(minimum) :
			(value >= maximum) ? static_cast<intmax_t>(maximum) :
			static_cast<intmax_t>(value);
	}

	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = false;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_CLAMP_POLICY_HPP_
