// bounded::integer-aware versions of the standard library algorithm functions
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

#ifndef BOUNDED_INTEGER_ALGORITHM_HPP_
#define BOUNDED_INTEGER_ALGORITHM_HPP_

#include "class.hpp"
#include "make.hpp"
#include "numeric_limits.hpp"
#include "operators/arithmetic.hpp"
#include "operators/comparison.hpp"

#include <iterator>

namespace bounded {

template<typename InputIterator, typename Predicate>
auto count_if(InputIterator first, InputIterator const last, Predicate predicate) {
	constexpr auto maximum = detail::basic_numeric_limits<typename std::iterator_traits<InputIterator>::difference_type>::max();
	integer<0, maximum> sum = make<0>();
	for (; first != last; ++first) {
		if (predicate(*first)) {
			++sum;
		}
	}
	return sum;
}

template<typename InputIterator, typename T>
auto count(InputIterator first, InputIterator last, T const & value) {
	return count_if(std::move(first), std::move(last), [&](decltype(*first) v) {
		return v == value;
	});
}

}	// namespace bounded
#endif	// BOUNDED_INTEGER_ALGORITHM_HPP_
