// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.find;

import containers.begin_end;
import containers.bidirectional_iterator;
import containers.bidirectional_range;
import containers.c_array;
import containers.empty_range;
import containers.is_iterator;
import containers.range;
import containers.sentinel_for;
import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<iterator Iterator>
constexpr auto find_if(Iterator first, sentinel_for<Iterator> auto const last, auto predicate) {
	for (; first != last; ++first) {
		if (predicate(*first)) {
			break;
		}
	}
	return first;
}

export constexpr auto find_if(range auto && range, auto predicate) {
	return ::containers::find_if(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


export template<bidirectional_iterator Iterator>
constexpr auto find_last_if(Iterator const first, Iterator const last, auto predicate) {
	for (auto it = last; it != first; ) {
		--it;
		if (predicate(*it)) {
			return it;
		}
	}
	return last;
}

export constexpr auto find_last_if(bidirectional_range auto && range, auto predicate) {
	return ::containers::find_last_if(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


export template<iterator Iterator>
constexpr auto find_if_not(Iterator first, sentinel_for<Iterator> auto const last, auto predicate) {
	return ::containers::find_if(std::move(first), last, std::not_fn(std::move(predicate)));
}

export constexpr auto find_if_not(range auto && range, auto predicate) {
	return ::containers::find_if_not(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


export template<bidirectional_iterator Iterator>
constexpr auto find_last_if_not(Iterator const first, Iterator const last, auto predicate) {
	return ::containers::find_last_if(first, last, std::not_fn(std::move(predicate)));
}

export constexpr auto find_last_if_not(bidirectional_range auto && range, auto predicate) {
	return ::containers::find_last_if_not(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		std::move(predicate)
	);
}


export template<iterator Iterator>
constexpr auto find(Iterator first, sentinel_for<Iterator> auto const last, auto const & value) {
	return ::containers::find_if(std::move(first), last, bounded::equal_to(value));
}

export constexpr auto find(range auto && range, auto const & value) {
	return ::containers::find(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		value
	);
}


export template<bidirectional_iterator Iterator>
constexpr auto find_last(Iterator const first, Iterator const last, auto const & value) {
	return ::containers::find_last_if(first, last, bounded::equal_to(value));
}

export constexpr auto find_last(bidirectional_range auto && range, auto const & value) {
	return ::containers::find_last(
		containers::begin(OPERATORS_FORWARD(range)),
		containers::end(OPERATORS_FORWARD(range)),
		value
	);
}

} // namespace containers

template<std::size_t size>
constexpr auto test(auto const index, containers::c_array<int, size> && a) -> bool {
	auto it = containers::find(a, 5);
	BOUNDED_ASSERT(index == containers::size(a) or *it == 5);
	return it - containers::begin(a) == index;
}

static_assert(containers::find(containers::empty_range<int>(), 5) == containers::empty_range<int>::end());
static_assert(test(1_bi, {0}));
static_assert(test(0_bi, {5}));
static_assert(test(2_bi, {0, 0}));
static_assert(test(1_bi, {0, 5}));
static_assert(test(0_bi, {5, 0}));
static_assert(test(0_bi, {5, 5}));


template<std::size_t size>
constexpr auto test_backward(auto const index, containers::c_array<int, size> && a) -> bool {
	auto it = containers::find_last(a, 5);
	BOUNDED_ASSERT(index == containers::size(a) or *it == 5);
	return it - containers::begin(a) == index;
}

static_assert(containers::find_last(containers::empty_range<int>(), 5) == containers::empty_range<int>::end());
static_assert(test_backward(1_bi, {0}));
static_assert(test_backward(0_bi, {5}));
static_assert(test_backward(2_bi, {0, 0}));
static_assert(test_backward(1_bi, {0, 5}));
static_assert(test_backward(0_bi, {5, 0}));
static_assert(test_backward(1_bi, {5, 5}));