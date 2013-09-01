// Factory function that turns a built-in into a ranged_integer
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

#ifndef RANGED_INTEGER_MAKE_RANGED_HPP_
#define RANGED_INTEGER_MAKE_RANGED_HPP_

#include "class.hpp"
#include "enable_if.hpp"
#include "policy.hpp"
#include <limits>

template<typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr ranged_integer<std::numeric_limits<integer>::min(), std::numeric_limits<integer>::max(), null_policy> make_ranged(integer const value) noexcept {
	return ranged_integer<std::numeric_limits<integer>::min(), std::numeric_limits<integer>::max(), null_policy>(value, non_check);
}

template<intmax_t integer>
constexpr ranged_integer<integer, integer, null_policy> make_ranged() noexcept {
	return ranged_integer<integer, integer, null_policy>{};
}

#endif	// RANGED_INTEGER_MAKE_RANGED_HPP_
