// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>
#include <containers/mutable_iterator.hpp>

#include <bounded/integer.hpp>

#include <cassert>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// TODO: conditional noexcept
template<typename Container, typename Iterator, BOUNDED_REQUIRES(is_container<Container> and is_iterator<Iterator>)>
constexpr auto erase(Container & container, Iterator const first_, Iterator const last_) noexcept {
	auto const first = ::containers::detail::mutable_iterator(container, first_);
	auto const last = ::containers::detail::mutable_iterator(container, last_);
	auto const to_clear = ::containers::move(last, end(container), first).output;
	while (to_clear != end(container)) {
		container.pop_back();
	}
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	assert(it != end(container));
	erase(container, it, ::containers::next(it));
}

template<typename Container, typename Predicate, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase_if(Container & container, Predicate predicate) {
	erase(container, ::containers::remove_if(begin(container), end(container), std::move(predicate)), end(container));
}


}	// namespace common
}	// namespace detail
}	// namespace containers
