// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

constexpr auto negate(auto predicate) {
	return [predicate = std::move(predicate)](auto && ... args) {
		return !predicate(BOUNDED_FORWARD(args)...);
	};
}

}	// namespace containers
