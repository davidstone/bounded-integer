// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

struct default_begin_end {
	static constexpr auto get_begin(auto && range) {
		return containers::begin(OPERATORS_FORWARD(range));
	}

	static constexpr auto get_end(auto && range) {
		return containers::end(OPERATORS_FORWARD(range));
	}
};

} // namespace containers
