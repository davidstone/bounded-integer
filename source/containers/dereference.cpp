// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.dereference;

import containers.c_array;
import containers.is_container;
import containers.iterator_t;

import bounded;
import std_module;

namespace containers {

export template<typename Range>
constexpr auto dereference = [](iterator_t<Range> const & it) -> decltype(auto) {
	if constexpr (is_container<Range>) {
		static_assert(std::is_lvalue_reference_v<decltype(*it)>);
		return std::move(*it);
	} else {
		return *it;
	}
};

} // namespace containers

using namespace bounded::literal;

static_assert(std::same_as<
	decltype(containers::dereference<containers::c_array<int, 1>>({})),
	int &&
>);
static_assert(std::same_as<
	decltype(containers::dereference<containers::c_array<int, 1> const &>({})),
	int const &
>);
static_assert(std::same_as<
	decltype(containers::dereference<containers::c_array<int, 1> &>({})),
	int &
>);