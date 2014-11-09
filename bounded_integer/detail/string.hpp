// Convert to a std::string / std::wstring
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

#ifndef BOUNDED_INTEGER_STRING_HPP_
#define BOUNDED_INTEGER_STRING_HPP_

#include "forward_declaration.hpp"
#include <string>
#include <type_traits>

namespace bounded {

// Import to_string for the numeric types
using std::to_string;

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto to_string(integer<minimum, maximum, overflow_policy, storage> const & x) {
	return to_string(+x.value());
}

using std::to_wstring;

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto to_wstring(integer<minimum, maximum, overflow_policy, storage> const & x) {
	return to_wstring(+x.value());
}

// TODO: template<typename T> T from_string(std::string const & str);

}	// namespace bounded

#endif	// BOUNDED_INTEGER_STRING_HPP_
