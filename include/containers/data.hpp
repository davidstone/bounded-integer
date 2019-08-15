// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

template<typename Range> requires is_range<Range>
constexpr auto data(Range && range) noexcept {
	if constexpr (requires { range.data(); }) {
		static_assert(noexcept(range.data()));
		return range.data();
	} else {
		static_assert(noexcept(pointer_from(begin(range))));
		return pointer_from(begin(range));
	}
}

}	// namespace common

using ::containers::detail::common::data;

}	// namespace detail

using ::containers::detail::common::data;

}	// namespace containers
