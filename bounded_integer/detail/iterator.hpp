// bounded::integer-aware versions of the standard library iterator functions
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

#ifndef BOUNDED_INTEGER_ITERATOR_HPP_
#define BOUNDED_INTEGER_ITERATOR_HPP_

#include "class.hpp"
#include "make.hpp"
#include "operators/arithmetic.hpp"

#include <iterator>

namespace bounded {

// These are not more specialized than std::next and std::prev. I could write a
// more specific case for bounded::array to ensure it is picked up by
// unqualified calls to next, but such calls are unlikely to ever happen.

// TODO: constexpr when gcc supports it
template<typename Iterator, typename Offset = decltype(make<1>())>
auto next(Iterator it, Offset const offset = make<1>()) {
	std::advance(it, offset);
	return it;
}

template<typename Iterator, typename Offset = decltype(make<1>())>
auto prev(Iterator it, Offset const offset = make<1>()) {
	std::advance(it, -offset);
	return it;
}

}	// namespace bounded
#endif	// BOUNDED_INTEGER_ITERATOR_HPP_
