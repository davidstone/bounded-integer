// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>

namespace containers {

template<iterator InputIterator, iterator OutputIterator>
constexpr auto copy(InputIterator first, sentinel_for<InputIterator> auto const last, OutputIterator out) {
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (; first != last; ++first, ++out) {
		*out = *first;
	}
	return result{std::move(first), std::move(out)};
}

template<iterator InputIterator, iterator OutputIterator>
constexpr auto copy(InputIterator in_first, sentinel_for<InputIterator> auto const in_last, OutputIterator out_first, sentinel_for<OutputIterator> auto const out_last) {
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (; in_first != in_last and out_first != out_last; ++in_first, ++out_first) {
		*out_first = *in_first;
	}
	return result{std::move(in_first), std::move(out_first)};
}

constexpr auto copy(range auto && input, iterator auto output) {
	return ::containers::copy(
		containers::begin(OPERATORS_FORWARD(input)),
		containers::end(OPERATORS_FORWARD(input)),
		std::move(output)
	);
}

constexpr auto copy(range auto && input, range auto && output) {
	return ::containers::copy(
		containers::begin(OPERATORS_FORWARD(input)),
		containers::end(OPERATORS_FORWARD(input)),
		containers::begin(OPERATORS_FORWARD(output)),
		containers::end(OPERATORS_FORWARD(output))
	);
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
	return ::containers::move(containers::begin(input), containers::end(input), out);
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto copy_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy(::containers::reverse_iterator(last), ::containers::reverse_iterator(first), ::containers::reverse_iterator(out_last)).output.base();
}

constexpr auto copy_backward(range auto && input, iterator auto out) {
	return ::containers::copy_backward(containers::begin(input), containers::end(input), out);
}

template<bidirectional_iterator BidirectionalIterator>
constexpr auto move_backward(BidirectionalIterator const first, BidirectionalIterator const last, bidirectional_iterator auto const out_last) {
	return ::containers::copy_backward(::containers::move_iterator(first), ::containers::move_iterator(last), out_last);
}

constexpr auto move_backward(range auto && input, iterator auto out) {
	return ::containers::move_backward(containers::begin(input), containers::end(input), out);
}

}	// namespace containers
