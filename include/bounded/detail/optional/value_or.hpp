// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/conditional.hpp>
#include <bounded/detail/forward.hpp>

#include <stdexcept>

namespace bounded {

constexpr decltype(auto) value_or(auto && value, auto && other) {
	return BOUNDED_CONDITIONAL(value, *BOUNDED_FORWARD(value), BOUNDED_FORWARD(other));
}

struct bad_optional_access : std::logic_error {
	bad_optional_access():
		std::logic_error("bad optional access") {
	}
};

constexpr decltype(auto) value(auto && value) {
	if (!value) {
		throw bad_optional_access{};
	}
	return *BOUNDED_FORWARD(value);
}

}	// namespace bounded
