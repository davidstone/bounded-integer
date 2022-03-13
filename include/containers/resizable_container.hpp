// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/erase_concepts.hpp>
#include <containers/push_back.hpp>

namespace containers {

template<typename T>
concept resizable_container = detail::push_backable<T> and detail::pop_backable<T> and range<T>;

} // namespace containers