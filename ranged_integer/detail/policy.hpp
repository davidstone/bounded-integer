// Ranged integer policies that determine what to do on overflow
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

#ifndef RANGED_INTEGER_POLICY_HPP_
#define RANGED_INTEGER_POLICY_HPP_

#include <cstdint>
#include <stdexcept>

// Default constructors are provided to work around
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#253

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

class throw_policy {
public:
	constexpr throw_policy() noexcept {}
	constexpr throw_policy(throw_policy const &) noexcept {}
	constexpr throw_policy(throw_policy &&) noexcept {}
	constexpr throw_policy(throw_policy const volatile &) noexcept {}
	constexpr throw_policy(throw_policy volatile &&) noexcept {}
	throw_policy & operator=(throw_policy const &) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy &&) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy const volatile &) noexcept {
		return *this;
	}
	throw_policy & operator=(throw_policy volatile &&) noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy const &) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy &&) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy const volatile &) volatile noexcept {
		return *this;
	}
	throw_policy volatile & operator=(throw_policy volatile &&) volatile noexcept {
		return *this;
	}
	// The optimizer should be able to simplify this to remove dead checks.
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const new_value) const {
		return (new_value < minimum) ?
			throw std::underflow_error{"Value too small"} :
			((new_value > maximum) ?
				throw std::overflow_error{"Value too large"} :
				new_value);
	}
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const new_value) const volatile {
		return (new_value < minimum) ?
			throw std::underflow_error{"Value too small"} :
			((new_value > maximum) ?
				throw std::overflow_error{"Value too large"} :
				new_value);
	}
	
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

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

#endif	// RANGED_INTEGER_POLICY_HPP_
