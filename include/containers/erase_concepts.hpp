// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/appendable_from_capacity.hpp>
#include <containers/has_member_before_begin.hpp>
#include <containers/is_range.hpp>
#include <containers/iterator_t.hpp>
#include <containers/splicable.hpp>

namespace containers::detail {

template<typename Container>
concept member_pop_backable = requires (Container & container) { container.pop_back(); };

template<typename Container>
concept member_pop_frontable = requires (Container & container) { container.pop_front(); };

template<typename Container>
concept member_erasable = requires(Container & container, iterator_t<Container const &> const it) {
	container.erase(it, it);
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

template<typename Container>
concept erasable = range<Container> and (
	member_erasable<Container> or
	splicable<Container> or
	appendable_from_capacity<Container>
);

} // namespace containers::detail
