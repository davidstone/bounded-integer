// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>

#include <bounded/forward.hpp>
#include <bounded/integer.hpp>
#include <bounded/is_constructible.hpp>

namespace containers {

// Works with explicit copy constructors
template<typename T> requires bounded::is_constructible<std::decay_t<T>, T &&>
constexpr auto copy(T && value) {
	return std::decay_t<T>(BOUNDED_FORWARD(value));
}

template<iterator InputIterator>
constexpr auto copy(InputIterator first, sentinel_for<InputIterator> auto const last, iterator auto out) {
	struct result {
		InputIterator input;
		decltype(out) output;
	};
	for (; first != last; ++first) {
		*out = *first;
		++out;
	}
	return result { first, out };
}

template<iterator InputIterator>
constexpr auto move(InputIterator const first, sentinel_for<InputIterator> auto const last, iterator auto const out) {
	auto const iterators = ::containers::copy(::containers::move_iterator(first), ::containers::move_iterator(last), out);
	struct result {
		InputIterator input;
		decltype(out) output;
	};
	return result { iterators.input.base(), iterators.output };
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto copy_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy(::containers::reverse_iterator(last), ::containers::reverse_iterator(first), ::containers::reverse_iterator(out_last)).output.base();
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto move_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy_backward(::containers::move_iterator(first), ::containers::move_iterator(last), out_last);
}

}	// namespace containers
