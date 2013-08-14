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

template<intmax_t minimum, intmax_t maximum>
class null_policy {
public:
	template<typename integer>
	constexpr integer operator()(integer const new_value) const noexcept {
		return new_value;
	}
};

template<intmax_t minimum, intmax_t maximum>
class throw_on_overflow {
public:
	template<typename integer>
	constexpr integer operator()(integer const new_value) const {
		return (new_value < minimum) ?
			throw std::underflow_error{"Value too small"} :
			((new_value > maximum) ?
				throw std::overflow_error{"Value too large"} :
				new_value);
	}
};


#endif	// RANGED_INTEGER_POLICY_HPP_
