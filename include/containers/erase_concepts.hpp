// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/appendable_from_capacity.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/iterator_t.hpp>

namespace containers::detail {

template<typename Container>
concept member_pop_backable = requires (Container & container) { container.pop_back(); };

template<typename Container>
concept member_pop_frontable = requires (Container & container) { container.pop_front(); };

// splice is required to be constant time
template<typename Container>
concept splicable = requires(Container target, iterator_t<Container const &> position, Container source) {
	target.splice(position, source);
};

// `erase_after` is required to be constant time
template<typename Container>
concept has_member_erase_after = requires (Container & container, iterator_t<Container &> it) {
	container.erase_after(it);
};

template<typename Container>
concept constant_time_erasable = splicable<Container>;

template<typename Container>
concept pop_backable =
	range<Container> and (
		member_pop_backable<Container> or
		appendable_from_capacity<Container> or
		(constant_time_erasable<Container> and bidirectional_range<Container>)
	);

template<typename Container>
concept pop_frontable =
	range<Container> and (
		member_pop_frontable<Container> or
		constant_time_erasable<Container> or
		(has_member_erase_after<Container> and has_member_before_begin<Container>)
	);

} // namespace containers::detail
