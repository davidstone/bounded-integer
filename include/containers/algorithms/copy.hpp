// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/iterator.hpp>

#include <bounded/integer.hpp>
#include <bounded/integer_range.hpp>

namespace containers {

// Works with explicit copy constructors
template<typename T>
constexpr auto copy(T && value) BOUNDED_NOEXCEPT_VALUE(
	std::decay_t<T>(std::forward<T>(value))
)


template<typename InputIterator, typename Sentinel, typename OutputIterator>
constexpr auto copy(InputIterator first, Sentinel const last, OutputIterator out) {
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (; first != last; ++first) {
		*out = *first;
		++out;
	}
	return result { first, out };
}

template<typename InputIterator, typename Sentinel, typename OutputIterator>
constexpr auto move(InputIterator const first, Sentinel const last, OutputIterator const out) {
	auto const iterators = ::containers::copy(::containers::move_iterator(last), ::containers::move_iterator(first), out);
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	return result { iterators.input.base(), iterators.output };
}

template<typename BidirectionalIterator, typename BidirectionalOutput>
constexpr auto copy_backward(BidirectionalIterator const first, BidirectionalIterator const last, BidirectionalOutput const out_last) BOUNDED_NOEXCEPT(
	::containers::copy(::containers::reverse_iterator(last), ::containers::reverse_iterator(first), ::containers::reverse_iterator(out_last)).output.base()
)

template<typename BidirectionalIterator, typename BidirectionalOutput>
constexpr auto move_backward(BidirectionalIterator const first, BidirectionalIterator const last, BidirectionalOutput const out_last) BOUNDED_NOEXCEPT(
	::containers::copy_backward(::containers::move_iterator(first), ::containers::move_iterator(last), out_last)
)


// Ideally this would be implemented with a counted iterator
template<typename InputIterator, typename Size, typename OutputIterator>
constexpr auto copy_n(InputIterator first, Size const count, OutputIterator out) {
	static_assert(count >= 0_bi, "Negative numbers not supported.");
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (auto const n : bounded::integer_range(count)) {
		static_cast<void>(n);
		*out = *first;
		++out;
		++first;
	}
	return result { first, out };
}

}	// namespace containers
