// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>

namespace containers {

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

constexpr auto copy(range auto && input, iterator auto out) {
	return ::containers::copy(begin(input), end(input), out);
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

constexpr auto move(range auto && input, iterator auto out) {
	return ::containers::move(begin(input), end(input), out);
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto copy_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy(::containers::reverse_iterator(last), ::containers::reverse_iterator(first), ::containers::reverse_iterator(out_last)).output.base();
}

constexpr auto copy_backward(range auto && input, iterator auto out) {
	return ::containers::copy_backward(begin(input), end(input), out);
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto move_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy_backward(::containers::move_iterator(first), ::containers::move_iterator(last), out_last);
}

constexpr auto move_backward(range auto && input, iterator auto out) {
	return ::containers::move_backward(begin(input), end(input), out);
}

}	// namespace containers
