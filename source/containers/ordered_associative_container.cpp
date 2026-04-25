// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.ordered_associative_container;

import containers.associative_container;

namespace containers {

template<typename T>
concept has_key_comp = requires(T const & map) {
	map.key_comp();
};

template<typename T>
concept has_extract_key = requires(T const & map) {
	map.extract_key();
};

export template<typename T>
concept ordered_associative_range =
	associative_range<T> and
	(has_key_comp<T> or has_extract_key<T>);

export template<typename T>
concept ordered_associative_container = ordered_associative_range<T> and associative_container<T>;

} // namespace containers
