// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/adapt.hpp>
#include <containers/algorithms/transform_iterator.hpp>

namespace containers {

constexpr auto transform(range auto && source, auto dereference) {
	return adapt(BOUNDED_FORWARD(source), detail::transform_traits(std::move(dereference)));
}

constexpr auto transform_dereference(range auto && source, auto dereference) {
	return adapt(BOUNDED_FORWARD(source), detail::transform_traits_dereference(std::move(dereference)));
}

}	// namespace containers
