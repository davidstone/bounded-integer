// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {

// Works with explicit copy constructors
template<typename T>
constexpr auto copy(T && value) BOUNDED_NOEXCEPT_VALUE(
	std::decay_t<T>(BOUNDED_FORWARD(value))
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
	auto const iterators = ::containers::copy(::containers::move_iterator(first), ::containers::move_iterator(last), out);
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

}	// namespace containers
