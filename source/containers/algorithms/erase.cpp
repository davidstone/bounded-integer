// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.erase;

import containers.algorithms.advance;
import containers.algorithms.destroy_range;
import containers.algorithms.find;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.can_set_size;
import containers.count_type;
import containers.erase_concepts;
import containers.iterator_t;
import containers.mutable_iterator;
import containers.range_view;
import containers.size;
import containers.splicable;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<erasable Container>
constexpr auto erase(Container & container, iterator_t<Container const &> const first, iterator_t<Container const &> const middle_) {
	if constexpr (member_erasable<Container>) {
		return container.erase(first, middle_);
	} else if constexpr (splicable<Container>) {
		auto temp = Container();
		temp.splice(containers::begin(temp), container, first, middle_);
		return mutable_iterator(container, middle_);
	} else {
		auto const middle = ::containers::mutable_iterator(container, middle_);
		if (first == middle) {
			return middle;
		}
		auto const offset = first - containers::begin(container);
		auto const count = middle - first;
		auto const last = containers::end(container);
		auto target = ::containers::mutable_iterator(container, first);
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

export template<erasable Container>
constexpr auto erase(Container & container, iterator_t<Container const &> const it) {
	BOUNDED_ASSERT(it != containers::end(container));
	return erase(container, it, ::containers::next(it));
}

export template<erasable Container>
constexpr void erase_to_end(Container & container, iterator_t<Container const &> const_position) {
	auto const last = containers::end(container);
	if constexpr (can_set_size<Container>) {
		auto const count = last - const_position;
		auto target = ::containers::mutable_iterator(container, const_position);
		containers::destroy_range(range_view(target, count));
		container.set_size(containers::size(container) - count);
	} else {
		::containers::erase(container, const_position, last);
	}
}

export template<erasable Container>
constexpr auto erase_if(Container & container, auto predicate) {
	auto result = count_type<Container>(0_bi);
	if constexpr (constant_time_erasable<Container>) {
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
		containers::erase_to_end(container, new_last);
	}
	return result;
}

export template<has_member_erase_after Container>
constexpr void erase_after(Container & container, iterator_t<Container const &> const before_first, iterator_t<Container const &> const last) {
	auto it = containers::next(before_first);
	while (it != last) {
		it = container.erase_after(before_first);
	}
}

} // namespace containers
