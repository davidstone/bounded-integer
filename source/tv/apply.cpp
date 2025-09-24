// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.apply;

import tv.tuple;
import tv.tuple_size;

import bounded;
import std_module;

namespace tv {

struct apply_t {
	static constexpr auto operator()(tuple_like auto && tuple_args, auto && function) -> decltype(auto) {
		auto && [...args] = OPERATORS_FORWARD(tuple_args);
		return OPERATORS_FORWARD(function)(OPERATORS_FORWARD(args)...);
	}
};
export constexpr auto apply = apply_t();

} // namespace tv
