// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.mutable_iterator;

import containers.array;
import containers.begin_end;
import containers.is_range;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.iterator_t;

import bounded;
import std_module;

namespace containers {

template<typename Container, typename Iterator>
concept has_member_mutable_iterator = requires(Container & container, Iterator it) {
	container.mutable_iterator(std::move(it));
};

template<typename Container, typename Iterator>
concept mutable_container_for =
	(!std::is_const_v<Container> and random_access_sentinel_for<Iterator, iterator_t<Container &>>) or
	(range<Container> and has_member_mutable_iterator<Container, Iterator>);

export template<typename Iterator, mutable_container_for<Iterator> Container>
constexpr auto mutable_iterator(Container & container, Iterator it) {
	if constexpr (has_member_mutable_iterator<Container, Iterator>) {
		return container.mutable_iterator(std::move(it));
	} else {
		return containers::begin(container) + (std::move(it) - containers::begin(container));
	}
}

} // namespace containers

using container = containers::array<int, bounded::constant<1>>;

template<typename Range, typename Iterator>
concept can_get_mutable_iterator_from = requires(Iterator it) {
	{ containers::mutable_iterator(bounded::declval<Range>(), it) } -> std::same_as<containers::iterator_t<container &>>;
};

template<typename Range>
concept can_get_mutable_iterator =
	can_get_mutable_iterator_from<Range, decltype(containers::begin(bounded::declval<container const &>()))> and
	can_get_mutable_iterator_from<Range, decltype(containers::begin(bounded::declval<container &>()))>;

static_assert(!can_get_mutable_iterator<container const &>);
static_assert(!can_get_mutable_iterator<container &&>);
static_assert(can_get_mutable_iterator<container &>);
