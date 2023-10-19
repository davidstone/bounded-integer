// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.erase_concepts;

import containers.can_set_size;
import containers.has_member_before_begin;
import containers.iterator_t;
import containers.range;
import containers.splicable;

import std_module;

namespace containers {

export template<typename Container>
concept member_pop_frontable = requires (Container & container) { container.pop_front(); };

export template<typename Container>
concept member_erasable = requires(Container & container, iterator_t<Container const &> const it) {
	container.erase(it, it);
};

// `erase_after` is required to be constant time
export template<typename T>
concept has_member_erase_after = requires(T & container, iterator_t<T const &> const it) {
	{ container.erase_after(it) } -> std::same_as<iterator_t<T &>>;
};

export template<typename Container>
concept constant_time_erasable = splicable<Container>;

export template<typename Container>
concept pop_frontable =
	range<Container> and (
		member_pop_frontable<Container> or
		constant_time_erasable<Container> or
		(has_member_erase_after<Container> and has_member_before_begin<Container>)
	);

export template<typename Container>
concept erasable = range<Container> and (
	member_erasable<Container> or
	splicable<Container> or
	can_set_size<Container>
);

} // namespace containers
