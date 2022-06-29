// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/destroy_range.hpp>
#include <containers/algorithms/find.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/can_set_size.hpp>
#include <containers/count_type.hpp>
#include <containers/erase_concepts.hpp>
#include <containers/iterator_t.hpp>
#include <containers/mutable_iterator.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>
#include <bounded/relocate.hpp>

#include <utility>

namespace containers {

template<detail::erasable Container>
constexpr auto erase(Container & container, iterator_t<Container const &> const first, iterator_t<Container const &> const middle_) {
	if constexpr (detail::member_erasable<Container>) {
		return container.erase(first, middle_);
	} else if constexpr (detail::splicable<Container>) {
		auto temp = Container();
		temp.splice(containers::begin(temp), container, first, middle_);
		return mutable_iterator(container, middle_);
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
			bounded::construct_at(*target, [&]{ return bounded::relocate(*source); });
			++target;
			++source;
		}
		if (source != last) {
			containers::uninitialized_relocate(range_view(source, last), middle);
		} else {
			containers::destroy_range(range_view(target, middle));
		}
		container.set_size(containers::size(container) - count);
		return containers::begin(container) + offset;
	}
}

template<detail::erasable Container>
constexpr auto erase(Container & container, iterator_t<Container const &> const it) {
	BOUNDED_ASSERT(it != containers::end(container));
	return erase(container, it, ::containers::next(it));
}

template<detail::erasable Container>
constexpr void erase_after(Container & container, iterator_t<Container const &> const_position) {
	auto const last = containers::end(container);
	if constexpr (detail::can_set_size<Container>) {
		auto const count = last - const_position;
		auto target = ::containers::detail::mutable_iterator(container, const_position);
		containers::destroy_range(range_view(target, last));
		container.set_size(containers::size(container) - count);
	} else {
		::containers::erase(container, const_position, last);
	}
}

template<detail::erasable Container>
constexpr auto erase_if(Container & container, auto predicate) {
	auto result = count_type<Container>(0_bi);
	if constexpr (detail::constant_time_erasable<Container>) {
		for (auto it = containers::begin(container); it != containers::end(container); ) {
			if (predicate(*it)) {
				it = ::containers::erase(container, it);
				++result;
			} else {
				++it;
			}
		}
	} else {
		auto const first = containers::begin(container);
		auto const last = containers::end(container);
		auto new_last = ::containers::find_if(first, last, predicate);
		if (new_last == last) {
			return result;
		}
		for (auto it = ::containers::next(new_last); it != last; ++it) {
			if (!predicate(*it)) {
				// TODO: Relocate?
				*new_last = std::move(*it);
				++new_last;
				++result;
			}
		}
		containers::erase_after(container, new_last);
	}
	return result;
}

} // namespace containers
