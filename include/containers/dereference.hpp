// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/iterator_t.hpp>

#include <type_traits>
#include <utility>

namespace containers {

template<typename Range>
inline constexpr auto dereference = [](iterator_t<Range> const & it) -> decltype(auto) {
	if constexpr (is_container<Range>) {
		static_assert(std::is_lvalue_reference_v<decltype(*it)>);
		return std::move(*it);
	} else {
		return *it;
	}
};

} // namespace containers
