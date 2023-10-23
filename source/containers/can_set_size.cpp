// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.can_set_size;

import containers.range;
import containers.range_size_t;

namespace containers {

export template<typename Container>
concept can_set_size =
	range<Container> and
	requires(Container & container, range_size_t<Container> const new_size) {
		container.set_size(new_size);
	};

} // namespace containers

static_assert(!containers::can_set_size<int>);

struct range_has_set_size {
	auto begin() const -> int const *;
	auto size() const -> int;
	auto set_size(int) -> void;
};

struct non_range_has_set_size {
	auto set_size(int) -> void;
};

static_assert(containers::can_set_size<range_has_set_size>);
static_assert(!containers::can_set_size<non_range_has_set_size>);