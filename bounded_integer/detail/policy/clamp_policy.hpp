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

#include <cstdint>

namespace bounded_integer {

class clamp_policy {
public:
	constexpr clamp_policy() noexcept {}
	constexpr clamp_policy(clamp_policy const &) noexcept {}
	constexpr clamp_policy(clamp_policy &&) noexcept {}
	constexpr clamp_policy(clamp_policy const volatile &) noexcept {}
	constexpr clamp_policy(clamp_policy volatile &&) noexcept {}
	clamp_policy & operator=(clamp_policy const &) noexcept {
		return *this;
	}
	clamp_policy & operator=(clamp_policy &&) noexcept {
		return *this;
	}
	clamp_policy & operator=(clamp_policy const volatile &) noexcept {
		return *this;
	}
	clamp_policy & operator=(clamp_policy volatile &&) noexcept {
		return *this;
	}
	clamp_policy volatile & operator=(clamp_policy const &) volatile noexcept {
		return *this;
	}
	clamp_policy volatile & operator=(clamp_policy &&) volatile noexcept {
		return *this;
	}
	clamp_policy volatile & operator=(clamp_policy const volatile &) volatile noexcept {
		return *this;
	}
	clamp_policy volatile & operator=(clamp_policy volatile &&) volatile noexcept {
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
