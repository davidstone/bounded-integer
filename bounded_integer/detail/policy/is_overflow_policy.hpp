// Determines if a class meets the requirements of an overflow policy.
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

#ifndef BOUNDED_INTEGER_POLICY_IS_OVERFLOW_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_IS_OVERFLOW_POLICY_HPP_

#include <cstdint>
#include <type_traits>

namespace bounded {

namespace detail {
namespace is_overflow_policy {

using yes = char[1];
using no = char[2];

#define BOUNDED_INTEGER_HAS_NESTED_TYPE(type) \
\
template<typename T> \
auto checker(typename T::type *) -> yes &; \
\
template<typename> \
auto checker(...) -> no &;

BOUNDED_INTEGER_HAS_NESTED_TYPE(overflow_policy_tag)
#undef BOUNDED_INTEGER_HAS_NESTED_TYPE
}	// namespace is_overflow_policy
}	// namespace detail


template<typename overflow_policy>
struct is_overflow_policy : std::integral_constant<
	bool,
	sizeof(detail::is_overflow_policy::checker<overflow_policy>(nullptr)) == sizeof(detail::is_overflow_policy::yes)
>{};

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_IS_OVERFLOW_POLICY_HPP_
