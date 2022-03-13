// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/is_container.hpp>

#include <type_traits>

namespace containers {

template<typename T>
concept associative_range =
	forward_range<T> and
	requires {
		typename std::remove_reference_t<T>::key_type;
		typename std::remove_reference_t<T>::mapped_type;
	};

template<typename T>
concept associative_container = associative_range<T> and is_container<T>;

} // namespace containers