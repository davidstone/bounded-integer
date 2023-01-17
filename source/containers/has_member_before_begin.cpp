// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.has_member_before_begin;

namespace containers {

export template<typename Range>
concept has_member_before_begin = requires(Range range) {
	OPERATORS_FORWARD(range).before_begin();
};

} // namespace containers
