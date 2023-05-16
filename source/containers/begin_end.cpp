// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.begin_end;

import containers.algorithms.advance;
import containers.c_array;
import containers.has_member_before_begin;
import containers.has_member_size;
export import containers.contiguous_iterator;

import bounded;

import std_module;

namespace containers {

template<typename Range>
concept has_member_begin = requires(Range range) {
	OPERATORS_FORWARD(range).begin();
};

template<typename Range>
concept should_use_before_begin = has_member_before_begin<Range> and !has_member_begin<Range>;

export constexpr auto begin(has_member_begin auto && range) {
	return OPERATORS_FORWARD(range).begin();
}

export constexpr auto begin(should_use_before_begin auto && range) {
	return ::containers::next(OPERATORS_FORWARD(range).before_begin());
}

export template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> const & a) {
	return contiguous_iterator<T const, size>(a);
}

export template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> & a) {
	return contiguous_iterator<T, size>(a);
}

export template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> && a) {
	return contiguous_iterator<T, size>(a);
}

template<typename Range>
concept has_member_end = requires(Range range) {
	OPERATORS_FORWARD(range).end();
};

template<typename Range>
concept endable =
	has_member_end<Range> or
	(has_member_begin<Range> and has_member_size<Range>);

export template<endable Range>
constexpr auto end(Range && range) {
	if constexpr (has_member_end<Range>) {
		return OPERATORS_FORWARD(range).end();
	} else {
		static_assert(has_member_begin<Range> and has_member_size<Range>);
		return OPERATORS_FORWARD(range).begin() + range.size();
	}
}

export template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> const & a) {
	return contiguous_iterator<T const, size>(a + size);
}

export template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> & a) {
	return contiguous_iterator<T, size>(a + size);
}

export template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> && a) {
	return contiguous_iterator<T, size>(a + size);
}

} // namespace containers

using namespace bounded::literal;

constexpr int a[] = {0, 1, 2, 3, 4};

static_assert(*containers::begin(a) == 0);
static_assert(containers::end(a) == containers::begin(a) + 5_bi);
