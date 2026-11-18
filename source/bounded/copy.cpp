// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.copy;

import bounded.concepts;

import std_module;

namespace bounded {

// Works with explicit copy constructors
// TODO: make this a lambda after resolution of
// https://github.com/llvm/llvm-project/issues/59513
struct copy_t {
	template<typename T>
	static constexpr auto operator()(T && value) requires constructible_from<std::decay_t<T>, T &&> {
		return std::decay_t<T>(OPERATORS_FORWARD(value));
	}
};
export constexpr auto copy = copy_t();

} // namespace bounded
