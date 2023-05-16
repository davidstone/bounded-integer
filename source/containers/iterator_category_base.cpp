// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

export module containers.iterator_category_base;

import containers.is_iterator;

namespace containers {

template<typename T>
concept has_iterator_category = requires {
	typename T::iterator_category;
};

export template<typename Iterator>
struct iterator_category_base {
};

template<has_iterator_category Iterator>
struct iterator_category_base<Iterator> {
	using iterator_category = typename Iterator::iterator_category;
};

} // namespace containers
