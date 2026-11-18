// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.make_index_sequence;

import bounded.integer;

import std_module;

namespace bounded {

export template<auto n>
constexpr auto make_index_sequence(constant_t<n>) {
	return std::make_index_sequence<n>{};
}

} // namespace bounded