// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/appendable_from_capacity.hpp>
#include <containers/begin_end.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>
#include <bounded/relocate.hpp>

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

template<resizable_container Container>
constexpr void erase_after(Container & container, iterator_t<Container const &> const_position) {
	auto const last = containers::end(container);
	if constexpr (detail::appendable_from_capacity<Container>) {
		auto const count = last - const_position;
		auto target = ::containers::detail::mutable_iterator(container, const_position);
		containers::destroy_range(range_view(target, last));
		container.append_from_capacity(-count);
	} else {
		container.erase(const_position, last);
	}
}

template<detail::erasable Container>
constexpr auto erase(Container & container, typename Container::const_iterator const first, typename Container::const_iterator const middle_) {
	if constexpr (detail::member_erasable<Container>) {
		return container.erase(first, middle_);
	} else {
		auto const middle = ::containers::detail::mutable_iterator(container, middle_);
		if (first == middle) {
			return middle;
		}
		auto const offset = first - containers::begin(container);
		auto const count = middle - first;
		auto const last = containers::end(container);
		auto target = ::containers::detail::mutable_iterator(container, first);
		auto source = middle;
		// TODO: Write a `relocate` algorithm for this loop?
		while (target != middle and source != last) {
			bounded::destroy(*target);
			bounded::construct(*target, [&]{ return bounded::relocate(*source); });
			++target;
			++source;
		}
		if (source != last) {
			containers::uninitialized_relocate(range_view(source, last), middle);
		} else {
			containers::destroy_range(range_view(target, middle));
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
