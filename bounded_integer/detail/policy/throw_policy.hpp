// Throws an exception on overflow
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

#ifndef BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_

#include <cstdint>
#include <stdexcept>

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
		return
			(new_value < minimum) ? throw std::underflow_error{"Value too small"} :
			(new_value > maximum) ? throw std::overflow_error{"Value too large"} :
			new_value;
	}
	template<intmax_t minimum, intmax_t maximum, typename integer>
	constexpr integer assignment(integer const new_value) const volatile {
		return
			(new_value < minimum) ? throw std::underflow_error{"Value too small"} :
			(new_value > maximum) ? throw std::overflow_error{"Value too large"} :
			new_value;
	}
	
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

#endif	// BOUNDED_INTEGER_POLICY_THROW_POLICY_HPP_
