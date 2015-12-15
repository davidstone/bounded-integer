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

#pragma once

#include <containers/is_iterator_sentinel.hpp>

#include <bounded_integer/bounded_integer.hpp>

namespace containers {

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto lexicographical_compare(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate compare) noexcept(
	noexcept(first1 != last1 and first2 != last2) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(!compare(*first1, *first2)) and
	noexcept(first1 == last1 and first2 != last2)
) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		auto const & value1 = *first1;
		auto const & value2 = *first2;
		if (compare(value1, value2)) {
			return true;
		}
		if (compare(value2, value1)) {
			return false;
		}
	}
	return first1 == last1 and first2 != last2;
}

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto lexicographical_compare(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare(first1, last1, first2, last2, bounded::less())
)


}	// namespace containers
