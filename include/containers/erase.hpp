// Copyright David Stone 2018.
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
#include <containers/pop_back.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
namespace common {

// TODO: this implementation is for contiguous ranges
template<container Container, iterator Iterator>
constexpr auto erase(Container & container, Iterator const first_, Iterator const last_) {
	auto const first = ::containers::detail::mutable_iterator(container, first_);
	auto const last = ::containers::detail::mutable_iterator(container, last_);
	auto const to_clear = ::containers::move(last, end(container), first).output;
	while (to_clear != end(container)) {
		pop_back(container);
	}
	return first;
}
template<container Container>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	BOUNDED_ASSERT(it != end(container));
	return erase(container, it, ::containers::next(it));
}

template<container Container, typename Predicate>
constexpr auto erase_if(Container & container, Predicate predicate) {
	return erase(container, ::containers::remove_if(container, std::move(predicate)), end(container));
}


}	// namespace common

using ::containers::detail::common::erase;
using ::containers::detail::common::erase_if;

}	// namespace detail

using ::containers::detail::common::erase;
using ::containers::detail::common::erase_if;

}	// namespace containers
