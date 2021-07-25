// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>

#include <type_traits>

namespace containers {

template<typename T, typename Source>
concept initializer_range =
	!std::is_same_v<std::remove_cvref_t<T>, Source> and
	!std::is_array_v<std::remove_cvref_t<T>> and
	range<T>;

} // namespace containers
