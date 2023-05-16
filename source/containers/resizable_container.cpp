// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.resizable_container;

import containers.is_range;
import containers.pop_back;
import containers.push_back;

namespace containers {

export template<typename T>
concept resizable_container = push_backable<T> and pop_backable<T> and range<T>;

} // namespace containers
