// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>
#include <containers/begin_end.hpp>

#include <operators/forward.hpp>

#include <memory>

namespace containers {

constexpr auto lookup(associative_container auto && container, auto && key) {
	auto const it = container.find(OPERATORS_FORWARD(key));
	return it != containers::end(container) ?
		std::addressof(get_mapped(*it)) :
		nullptr;
}

} // namespace containers
