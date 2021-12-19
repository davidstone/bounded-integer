// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/negate.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterator.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

#include <type_traits>
#include <utility>

namespace containers {

template<iterator Iterator>
constexpr auto find_if(Iterator first, sentinel_for<Iterator> auto const last, auto predicate) {
	for (; first != last; ++first) {
		if (predicate(*first)) {
			break;
		}
	}
	return first;
}

constexpr auto find_if(range auto && range, auto predicate) {
	return ::containers::find_if(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


template<bidirectional_iterator Iterator>
constexpr auto find_last_if(Iterator const first, Iterator const last, auto predicate) {
	for (auto it = last; it != first; ) {
		--it;
		if (predicate(*it)) {
			return it;
		}
	}
	return last;
}

constexpr auto find_last_if(bidirectional_range auto && range, auto predicate) {
	return ::containers::find_last_if(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


template<iterator Iterator>
constexpr auto find_if_not(Iterator const first, sentinel_for<Iterator> auto const last, auto predicate) {
	return ::containers::find_if(first, last, ::containers::negate(std::move(predicate)));
}

constexpr auto find_if_not(range auto && range, auto predicate) {
	return ::containers::find_if_not(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


template<bidirectional_iterator Iterator>
constexpr auto find_last_if_not(Iterator const first, Iterator const last, auto predicate) {
	return ::containers::find_last_if(first, last, ::containers::negate(std::move(predicate)));
}

constexpr auto find_last_if_not(bidirectional_range auto && range, auto predicate) {
	return ::containers::find_last_if_not(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


template<iterator Iterator>
constexpr auto find(Iterator const first, sentinel_for<Iterator> auto const last, auto const & value) {
	return ::containers::find_if(first, last, bounded::equal_to(value));
}

constexpr auto find(range auto && range, auto const & value) {
	return ::containers::find(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		value
	);
}


template<bidirectional_iterator Iterator>
constexpr auto find_last(Iterator const first, Iterator const last, auto const & value) {
	return ::containers::find_last_if(first, last, bounded::equal_to(value));
}

constexpr auto find_last(bidirectional_range auto && range, auto const & value) {
	return ::containers::find_last(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		value
	);
}

} // namespace containers
