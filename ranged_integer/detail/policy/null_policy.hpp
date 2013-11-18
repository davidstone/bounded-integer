// Does nothing. Overflow is undefined behavior
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

#ifndef RANGED_INTEGER_POLICY_NULL_POLICY_HPP_
#define RANGED_INTEGER_POLICY_NULL_POLICY_HPP_

#include <cstdint>

class null_policy {
public:
	constexpr null_policy() noexcept {}
	constexpr null_policy(null_policy const &) noexcept {}
	constexpr null_policy(null_policy &&) noexcept {}
	constexpr null_policy(null_policy const volatile &) noexcept {}
	constexpr null_policy(null_policy volatile &&) noexcept {}
	null_policy & operator=(null_policy const &) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy &&) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy const volatile &) noexcept {
		return *this;
	}
	null_policy & operator=(null_policy volatile &&) noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy const &) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy &&) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy const volatile &) volatile noexcept {
		return *this;
	}
	null_policy volatile & operator=(null_policy volatile &&) volatile noexcept {
		return *this;
	}
	template<intmax_t, intmax_t, typename integer>
	constexpr integer assignment(integer const new_value) const noexcept {
		return new_value;
	}
	template<intmax_t, intmax_t, typename integer>
	constexpr integer assignment(integer const new_value) const volatile noexcept {
		return new_value;
	}

	// It might actually be true! This should be considered undefined
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

#endif	// RANGED_INTEGER_POLICY_NULL_POLICY_HPP_
