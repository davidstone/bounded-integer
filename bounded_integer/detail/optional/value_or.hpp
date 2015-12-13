// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "../conditional.hpp"

#include <stdexcept>

namespace bounded {

template<typename Optional, typename Default>
constexpr decltype(auto) value_or(Optional && value, Default && other) {
	return BOUNDED_CONDITIONAL(value, *std::forward<Optional>(value), std::forward<Default>(other));
}

struct bad_optional_access : std::logic_error {
	bad_optional_access():
		std::logic_error("bad optional access") {
	}
};

template<typename Optional>
constexpr decltype(auto) value(Optional && value) {
	if (!value) {
		throw bad_optional_access{};
	}
	return *std::forward<Optional>(value);
}

}	// namespace bounded
