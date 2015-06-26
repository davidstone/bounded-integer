// Overload for stream insertion / extraction
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

#pragma once

#include "class.hpp"
#include <istream>
#include <ostream>

namespace bounded {

template<
	typename CharT, typename Traits,
	intmax_t minimum, intmax_t maximum, typename policy, storage_type storage
>
decltype(auto) operator<<(std::basic_ostream<CharT, Traits> & out, integer<minimum, maximum, policy, storage> const & x) {
	// The unary plus applies integer promotions to x. This ensures values are
	// printed as integers. Without this, I could run into an issue where the
	// underlying type is a typedef for signed char / unsigned char. This would
	// output the value as though it were a character.
	return out << +x.value();
}
template<
	typename CharT, typename Traits,
	intmax_t minimum, intmax_t maximum, typename policy, storage_type storage
>
decltype(auto) operator<<(std::basic_ostream<CharT, Traits> & out, integer<minimum, maximum, policy, storage> const volatile & x) {
	return out << +x.value();
}

template<
	typename CharT, typename Traits,
	intmax_t minimum, intmax_t maximum, typename policy, storage_type storage
>
decltype(auto) operator>>(std::basic_istream<CharT, Traits> & in, integer<minimum, maximum, policy, storage> & x) {
	// This is intmax_t rather than underlying_type to maximize the chances for
	// robust error checking rather than undefined behavior, but it still fails
	// for very large and very small numbers.
	intmax_t temp;
	in >> temp;
	x = temp;
	return in;
}
template<
	typename CharT, typename Traits,
	intmax_t minimum, intmax_t maximum, typename policy, storage_type storage
>
decltype(auto) operator>>(std::basic_istream<CharT, Traits> & in, integer<minimum, maximum, policy, storage> volatile & x) {
	intmax_t temp;
	in >> temp;
	x = temp;
	return in;
}

}	// namespace bounded
