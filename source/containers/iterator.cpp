// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iterator;

import bounded;
import std_module;

namespace containers {

export template<typename Iterator>
concept iterator = requires(std::remove_cvref_t<Iterator> it) {
	*it;
	++it;
};

} // namespace containers
