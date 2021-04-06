// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/erase.hpp>
#include <containers/append.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/clear.hpp>
#include <containers/is_range.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<resizable_container Target>
constexpr void assign(Target & target, range auto && source) {
	// Either of these implementations work for any type. For types that are not
	// trivially copyable, we want to reuse the existing storage where that
	// makes sense. For types that are trivially copyable, that concept doesn't
	// make sense, so we instead use code that optimizes better.
	if constexpr (std::is_trivially_copyable_v<range_value_t<Target>>) {
		::containers::clear(target);
		::containers::assign_to_empty(target, OPERATORS_FORWARD(source));
	} else {
		auto result = ::containers::copy(OPERATORS_FORWARD(source), OPERATORS_FORWARD(target));
		::containers::erase_after(target, std::move(result.output));
		::containers::append(target, range_view(std::move(result.input), containers::end(OPERATORS_FORWARD(source))));
	}
}
template<resizable_container Container, std::size_t init_size>
constexpr void assign(Container & destination, c_array<range_value_t<Container>, init_size> && init) {
	assign(destination, range_view(std::move(init)));
}

} // namespace containers
