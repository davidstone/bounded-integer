// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.maybe_find;

import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.iterator;
import containers.range;
import containers.sentinel_for;

import bounded;
import tv;
import std_module;

namespace containers {

// the maybe_find functions return a value contextually convertible to bool. If
// the range contains the element, it can be dereferenced as an optional
export template<iterator ForwardIterator>
constexpr auto maybe_find_if(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto predicate) {
	auto const it = ::containers::find_if(first, last, std::move(predicate));
	if constexpr (std::is_lvalue_reference_v<decltype(*it)>) {
		return it != last ? std::addressof(*it) : nullptr;
	} else {
		return it != last ? tv::make_optional(*it) : tv::none;
	}
}

export constexpr auto maybe_find_if(range auto && range, auto predicate) {
	return ::containers::maybe_find_if(containers::begin(OPERATORS_FORWARD(range)), containers::end(OPERATORS_FORWARD(range)), std::move(predicate));
}


export template<iterator ForwardIterator>
constexpr auto maybe_find(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto const & value) {
	return ::containers::maybe_find_if(first, last, bounded::equal_to(value));
}

export constexpr auto maybe_find(range auto && range, auto const & value) {
	return ::containers::maybe_find(containers::begin(OPERATORS_FORWARD(range)), containers::end(OPERATORS_FORWARD(range)), value);
}

} // namespace containers

using namespace bounded::literal;

constexpr auto values = containers::array{1, 2, 5, 9, 0};

static_assert(containers::maybe_find(values, 5) == std::addressof(values[2_bi]));
static_assert(!containers::maybe_find(values, 7));
static_assert(containers::maybe_find_if(values, [](auto const x) { return x % 2 == 0; }) == std::addressof(values[1_bi]));
