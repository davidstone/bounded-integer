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

#ifndef RANGED_INTEGER_MAKE_RANGED_OPTIONAL_HPP_
#define RANGED_INTEGER_MAKE_RANGED_OPTIONAL_HPP_

#include "class.hpp"
#include "make_ranged.hpp"
#include "optional.hpp"
#include "policy/null_policy.hpp"
#include <limits>
#include <type_traits>

// This is used to help make arrays of possibly optional ranged_integer. This
// returns a value that can be used to construct an optional using the copy
// constructor or the none_t constructor.

namespace detail {

template<typename overflow_policy, typename T>
using make_ranged_optional_result_t = typename std::conditional<
	std::is_same<T, none_t>::value,
	none_t,
	make_ranged_result_t<overflow_policy, T>
>::type;


template<
	typename overflow_policy = null_policy,
	typename T = void,
	typename result_type = make_ranged_optional_result_t<overflow_policy, T>
>
constexpr result_type make_ranged_optional(T const value) noexcept {
	return result_type(value);
}

}	// namespace detail

#endif	// RANGED_INTEGER_MAKE_RANGED_OPTIONAL_HPP_
