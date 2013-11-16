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

template<intmax_t minimum, intmax_t maximum>
class null_policy {
public:
	constexpr null_policy() noexcept {}
	template<typename integer>
	constexpr integer assignment(integer const new_value) const noexcept {
		return new_value;
	}

	// It might actually be true! This should be considered undefined
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

template<intmax_t minimum, intmax_t maximum>
class throw_on_overflow {
public:
	constexpr throw_on_overflow() noexcept {}
	// The optimizer should be able to simplify this to remove dead checks.
	template<typename integer>
	constexpr integer assignment(integer const new_value) const {
		return (new_value < minimum) ?
			throw std::underflow_error{"Value too small"} :
			((new_value > maximum) ?
				throw std::overflow_error{"Value too large"} :
				new_value);
	}
	
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

template<intmax_t minimum, intmax_t maximum>
class clamp_on_overflow {
public:
	constexpr clamp_on_overflow() noexcept {}
	template<typename integer>
	constexpr integer assignment(integer const new_value) const noexcept {
		return
			(new_value <= minimum) ? static_cast<integer>(minimum) :
			(new_value >= maximum) ? static_cast<integer>(maximum) :
			new_value;
	}

	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = false;
};

#endif	// RANGED_INTEGER_POLICY_HPP_
