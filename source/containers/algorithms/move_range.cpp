// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.move_range;

import containers.algorithms.transform;

import containers.range;

import std_module;

namespace containers {

struct move_transform_t {
	static constexpr auto operator()(auto const & it) -> decltype(auto) {
		using base_result = decltype(*it);
		using result = std::conditional_t<
			std::is_reference_v<base_result>,
			std::remove_reference_t<base_result> &&,
			base_result
		>;
		return static_cast<result>(*it);
	}
};

export template<range Range>
constexpr auto move_range(Range && r) {
	return containers::transform_dereference(OPERATORS_FORWARD(r), move_transform_t());
}

} // namespace containers
