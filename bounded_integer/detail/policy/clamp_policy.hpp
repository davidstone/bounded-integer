// Also known as saturation
// Copyright (C) 2013 David Stone
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
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const new_value) const noexcept {
		return
			(new_value <= minimum) ? static_cast<integer>(minimum) :
			(new_value >= maximum) ? static_cast<integer>(maximum) :
			new_value;
	}
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const new_value) const volatile noexcept {
		return
			(new_value <= minimum) ? static_cast<integer>(minimum) :
			(new_value >= maximum) ? static_cast<integer>(maximum) :
			new_value;
	}

	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = false;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_CLAMP_POLICY_HPP_
