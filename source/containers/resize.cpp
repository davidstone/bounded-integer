// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.resize;

import containers.algorithms.erase;
import containers.algorithms.generate;
import containers.append;
import containers.begin_end;
import containers.count_type;
import containers.forward_random_access_range;
import containers.pop_back;
import containers.range_value_t;
import containers.resizable_container;
import containers.size;

import bounded;

namespace containers {

template<typename Container>
constexpr auto resize_impl(Container & container, auto const new_size, auto const generator) {
	auto const container_size = bounded::integer(containers::size(container));
	if (new_size <= container_size) {
		if constexpr (forward_random_access_range<Container &>) {
			auto const it = containers::begin(container) + ::bounded::assume_in_range<containers::count_type<Container>>(new_size);
			containers::erase_to_end(container, it);
		} else {
			while (containers::size(container) != new_size) {
				containers::pop_back(container);
			}
		}
	} else {
		auto const remaining = bounded::increase_min<0>(new_size - container_size, bounded::unchecked);
		::containers::append(container, containers::generate_n(remaining, generator));
	}
}

export template<resizable_container Container>
constexpr auto resize(Container & container, auto const new_size) {
	::containers::resize_impl(container, bounded::integer(new_size), bounded::construct<range_value_t<Container>>);
}
export template<resizable_container Container>
constexpr auto resize(Container & container, auto const new_size, range_value_t<Container> const & value) {
	::containers::resize_impl(container, bounded::integer(new_size), bounded::value_to_function(value));
}

} // namespace containers
