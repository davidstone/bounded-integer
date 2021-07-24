// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform.hpp>
#include <containers/iterator_t.hpp>

#include <bounded/relocate.hpp>

#include <operators/forward.hpp>

#include <type_traits>

namespace containers::detail {

template<range Range>
constexpr auto relocate_range_adapter(Range && r) {
	return ::containers::transform_dereference(OPERATORS_FORWARD(r), [](iterator_t<Range> const & it) noexcept {
		if constexpr (std::is_reference_v<decltype(*it)>) {
			return bounded::relocate(*it);
		} else {
			return *it;
		}
	});
}

} // namespace containers
