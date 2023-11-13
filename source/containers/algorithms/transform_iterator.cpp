// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/returns.hpp>

export module containers.algorithms.transform_iterator;

import containers.algorithms.transform_traits;
import containers.default_adapt_traits;
import containers.is_iterator;
import containers.iterator_adapter;

import std_module;

namespace containers {

export constexpr auto transform_iterator(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), transform_traits(std::move(dereference)));
}

export constexpr auto transform_iterator_dereference(iterator auto it, auto dereference) {
	return adapt_iterator(std::move(it), transform_traits_dereference(std::move(dereference)));
}

} // namespace containers
