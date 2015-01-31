// More usable functions to change the bounds of an integer
// Copyright (C) 2015 David Stone
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

#ifndef BOUNDED_INTEGER_CAST_HPP_
#define BOUNDED_INTEGER_CAST_HPP_

#include "forward_declaration.hpp"
#include "noexcept.hpp"

#include <utility>

namespace bounded {

// Other args allow you to specify an overflow policy or declare the conversion as non_check
template<intmax_t new_minimum, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename... Args>
constexpr auto increase_min(integer<minimum, maximum, overflow_policy, storage> const & value, Args && ... args) BOUNDED_NOEXCEPT((
	integer<(new_minimum > minimum) ? new_minimum : minimum, maximum, overflow_policy, storage>(value, std::forward<Args>(args)...)
))

template<intmax_t new_maximum, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename... Args>
constexpr auto decrease_max(integer<minimum, maximum, overflow_policy, storage> const & value, Args && ... args) BOUNDED_NOEXCEPT((
	 integer<minimum, (new_maximum < maximum) ? new_maximum : maximum, overflow_policy, storage>(value, std::forward<Args>(args)...)
))

}	// namespace bounded
#endif	// BOUNDED_INTEGER_CAST_HPP_
