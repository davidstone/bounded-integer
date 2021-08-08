// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iterator_t.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<typename Container, typename Iterator>
concept has_member_mutable_iterator = requires(Container & container, Iterator it) {
	container.mutable_iterator(std::move(it));
};

template<typename Container, typename Iterator>
concept mutable_container_for =
	(!std::is_const_v<Container> and random_access_sentinel_for<Iterator, iterator_t<Container &>>) or
	(range<Container> and has_member_mutable_iterator<Container, Iterator>);

template<typename Iterator, mutable_container_for<Iterator> Container>
constexpr auto mutable_iterator(Container & container, Iterator it) {
	if constexpr (has_member_mutable_iterator<Container, Iterator>) {
		return container.mutable_iterator(std::move(it));
	} else {
		return containers::begin(container) + (std::move(it) - containers::begin(container));
	}
}

} // namespace detail
} // namespace containers
