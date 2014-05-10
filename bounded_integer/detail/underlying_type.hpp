// Determine the best underlying type to use to store the value
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

#ifndef BOUNDED_INTEGER_UNDERLYING_TYPE_HPP_
#define BOUNDED_INTEGER_UNDERLYING_TYPE_HPP_

#include <cstdint>
#include <type_traits>
#include <boost/integer.hpp>

namespace bounded {
namespace detail {

template<typename T1, typename T2>
using larger_type = std::conditional_t<sizeof(T1) >= sizeof(T2), T1, T2>;

#define BOUNDED_INTEGER_UNDERLYING_TYPE(version) \
template<intmax_t minimum, intmax_t maximum> \
using signed_ ## version ## _t = larger_type< \
	typename boost::int_min_value_t<minimum>::version, \
	typename boost::int_max_value_t<maximum>::version \
>; \
\
template<intmax_t minimum, intmax_t maximum> \
using unsigned_ ## version ## _t = typename boost::uint_value_t<static_cast<unsigned long long>(maximum)>::version; \
\
template<intmax_t minimum, intmax_t maximum> \
using version ## _t = std::conditional_t< \
	minimum < 0, \
	signed_ ## version ## _t<minimum, maximum>, \
	unsigned_ ## version ## _t<minimum, maximum> \
>;


BOUNDED_INTEGER_UNDERLYING_TYPE(fast)
BOUNDED_INTEGER_UNDERLYING_TYPE(least)



}	// namespace detail
}	// namespace bounded

#endif	// BOUNDED_INTEGER_UNDERLYING_TYPE_HPP_
