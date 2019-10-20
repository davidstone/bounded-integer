// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/negate.hpp>
#include <containers/is_iterator.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

#include <type_traits>
#include <utility>

namespace containers {

template<iterator InputIterator>
constexpr auto find_if(InputIterator first, sentinel_for<InputIterator> auto const last, auto predicate) {
	for (; first != last; ++first) {
		if (predicate(*first)) {
			break;
		}
	}
	return first;
}

constexpr auto find_if(range auto && range, auto predicate) {
	return ::containers::find_if(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), std::move(predicate));
}


template<iterator InputIterator>
constexpr auto find_if_not(InputIterator const first, sentinel_for<InputIterator> auto const last, auto predicate) {
	return ::containers::find_if(first, last, ::containers::negate(std::move(predicate)));
}

constexpr auto find_if_not(range auto && range, auto predicate) {
	return ::containers::find_if_not(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), std::move(predicate));
}


template<iterator InputIterator>
constexpr auto find(InputIterator const first, sentinel_for<InputIterator> auto const last, auto const & value) {
	return ::containers::find_if(first, last, bounded::equal_to(value));
}

constexpr auto find(range auto && range, auto const & value) {
	return ::containers::find(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), value);
}

}	// namespace containers
