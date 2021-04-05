// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/relocate.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
concept member_erasable = requires(Container & container, typename Container::const_iterator const it1, typename Container::const_iterator const it2) {
	container.erase(it1);
	container.erase(it1, it2);
};

template<typename Container>
concept erasable = member_erasable<Container> or resizable_container<Container>;

} // namespace detail

template<detail::erasable Container>
constexpr auto erase(Container & container, typename Container::const_iterator const first, typename Container::const_iterator const last) {
	if constexpr (detail::member_erasable<Container>) {
		return container.erase(first, last);
	} else {
		auto const offset = first - containers::begin(container);
		auto const count = last - first;
		auto const end_ = containers::end(container);
		auto target = ::containers::detail::mutable_iterator(container, first);
		auto source = ::containers::detail::mutable_iterator(container, last);
		// TODO: Write a `relocate` algorithm for this loop?
		while (target != last and source != end_) {
			bounded::destroy(*target);
			bounded::construct(*target, [&]{ return relocate(*source); });
			++target;
			++source;
		}
		if (source != end_) {
			containers::uninitialized_relocate(target, source, end_);
		} else {
			containers::destroy_range(target, last);
		}
		container.append_from_capacity(-count);
		return containers::begin(container) + offset;
	}
}

template<detail::erasable Container>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	if constexpr (detail::member_erasable<Container>) {
		return container.erase(it);
	} else {
		BOUNDED_ASSERT(it != containers::end(container));
		return erase(container, it, ::containers::next(it));
	}
}

constexpr auto erase_if(resizable_container auto & container, auto predicate) {
	return erase(container, ::containers::remove_if(container, std::move(predicate)), containers::end(container));
}

} // namespace containers
