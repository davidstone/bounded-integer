// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

export module containers.iterator_category_base;

import bounded;
import std_module;

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

template<has_iterator_category Iterator> requires(!bounded::convertible_to<typename Iterator::iterator_category, std::forward_iterator_tag>)
struct iterator_category_base<Iterator> {
	using iterator_category = typename Iterator::iterator_category;

	iterator_category_base() = default;
	iterator_category_base(iterator_category_base &&) = default;
	iterator_category_base(iterator_category_base const &) = delete;
	auto operator=(iterator_category_base &&) -> iterator_category_base & = default;
	auto operator=(iterator_category_base const &) -> iterator_category_base & = delete;
};

} // namespace containers
