// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.begin_end;

import containers.algorithms.advance;

import containers.c_array;
export import containers.contiguous_iterator;
import containers.data;
import containers.has_member_before_begin;
import containers.range;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

template<typename Range>
concept has_member_begin = requires(Range r) {
	OPERATORS_FORWARD(r).begin();
};

constexpr auto get_size(auto const & r) {
	return r.size();
}
template<typename T, std::size_t size>
constexpr auto get_size(c_array<T, size> const &) {
	return bounded::constant<size>;
}

export template<range Range>
constexpr auto begin(Range && r) {
	if constexpr (has_member_begin<Range>) {
		return OPERATORS_FORWARD(r).begin();
	} else if constexpr (has_member_before_begin<Range>) {
		return ::containers::next(OPERATORS_FORWARD(r).before_begin());
	} else {
		auto const ptr = containers::data(r);
		using T = std::remove_reference_t<decltype(*ptr)>;
		constexpr auto size = numeric_traits::max_value<decltype(get_size(r))>;
		return contiguous_iterator<T, size>(ptr);
	}
}

template<typename Range>
concept has_member_end = requires(Range r) {
	OPERATORS_FORWARD(r).end();
};

export template<range Range>
constexpr auto end(Range && r) {
	if constexpr (has_member_end<Range>) {
		return OPERATORS_FORWARD(r).end();
	} else {
		return containers::begin(OPERATORS_FORWARD(r)) + get_size(r);
	}
}

} // namespace containers

using namespace bounded::literal;

constexpr int a[] = {0, 1, 2, 3, 4};

static_assert(*containers::begin(a) == 0);
static_assert(containers::end(a) == containers::begin(a) + 5_bi);

struct {
	constexpr auto begin() const -> int const * {
		return nullptr;
	}
	constexpr auto end() const -> int const * {
		return nullptr;
	}
} constexpr begin_and_end;
static_assert(containers::begin(begin_and_end) == containers::end(begin_and_end));

struct {
	constexpr auto begin() const -> int const * {
		return nullptr;
	}
	constexpr auto size() const {
		return 0_bi;
	}
} constexpr begin_and_size;
static_assert(containers::begin(begin_and_size) == containers::end(begin_and_size));

struct {
	constexpr auto before_begin() const -> int const * {
		return a;
	}
	constexpr auto end() const -> int const * {
		return a + 1;
	}
} constexpr before_begin_and_end;
static_assert(containers::begin(before_begin_and_end) == containers::end(before_begin_and_end));

struct {
	constexpr auto data() const -> int const * {
		return nullptr;
	}
	constexpr auto size() const {
		return 0_bi;
	}
} constexpr data_and_size;
static_assert(containers::begin(data_and_size) == containers::end(data_and_size));
