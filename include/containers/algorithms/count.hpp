// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <iterator>
#include <limits>
#include <utility>

namespace containers {

using namespace bounded::literal;

template<typename InputIterator, typename Sentinel, typename Predicate>
constexpr auto count_if(InputIterator first, Sentinel const last, Predicate predicate) noexcept(
	noexcept(first != last) and noexcept(predicate(*first))
) {
	constexpr auto maximum = std::numeric_limits<typename std::iterator_traits<InputIterator>::difference_type>::max();
	bounded::integer<0, static_cast<std::uintmax_t>(maximum)> sum = 0_bi;
	for (; first != last; ++first) {
		if (predicate(*first)) {
			++sum;
		}
	}
	return sum;
}

template<typename InputIterator, typename Sentinel, typename T>
constexpr auto count(InputIterator first, Sentinel last, T const & value) BOUNDED_NOEXCEPT(
	::containers::count_if(std::move(first), std::move(last), bounded::equal_to(value))
)

}	// namespace containers
