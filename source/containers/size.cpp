// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.size;

import containers.begin_end;
import containers.c_array;
import containers.has_member_size;
import containers.range_size_t;
import containers.sized_range;

import bounded;
import std_module;

namespace containers {

export template<sized_range Range>
constexpr auto size(Range && range) {
	if constexpr (has_member_size<Range>) {
		return range.size();
	} else {
		return ::bounded::assume_in_range<range_size_t<Range>>(
			containers::end(OPERATORS_FORWARD(range)) - containers::begin(OPERATORS_FORWARD(range))
		);
	}
}

export template<typename T, std::size_t size_>
constexpr auto size(c_array<T, size_> const &) {
	return bounded::constant<size_>;
}

} // namespace containers
namespace {
using namespace bounded::literal;

constexpr int a[] = {0, 1, 2, 3, 4};
static_assert(containers::size(a) == 5_bi);

struct static_member_size {
	static constexpr auto begin() -> int const * {
		return nullptr;
	}
	static constexpr auto size() {
		return 1;
	}
};
void f(static_member_size parameter) {
	static_assert(containers::size(parameter) == 1);
}

struct member_size {
	constexpr auto begin() const -> int const * {
		return nullptr;
	}
	constexpr auto size() const {
		return 2;
	}
};
static_assert(containers::size(member_size()) == 2);

struct member_size_type {
	using size_type = int;
	constexpr auto begin() const -> int const * {
		return a;
	}
	constexpr auto end() const -> int const * {
		return begin() + 3;
	}
};
static_assert(containers::size(member_size_type()) == 3);

struct member_begin_end {
	constexpr auto begin() const -> int const * {
		return a;
	}
	constexpr auto end() const -> int const * {
		return begin() + 4;
	}
};
static_assert(containers::size(member_begin_end()) == 4);

} // namespace