// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.member_assign;

import containers.begin_end;

namespace containers {

template<typename Target, typename Source>
concept member_range_assignable = requires(Target & target, Source && source) {
	target.assign(OPERATORS_FORWARD(source));
};

template<typename Target, typename Source>
concept member_iterator_assignable = requires(Target & target, Source && source) {
	target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
};

export template<typename Target, typename Source>
concept member_assignable = member_range_assignable<Target, Source> or member_iterator_assignable<Target, Source>;

export template<typename Target, typename Source>
constexpr auto member_assign(Target & target, Source && source) -> void {
	if constexpr (member_range_assignable<Target, Source>) {
		target.assign(OPERATORS_FORWARD(source));
	} else {
		static_assert(member_iterator_assignable<Target, Source>);
		// TODO: Reserve for sized forward input ranges
		target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
	}
};

} // namespace containers
