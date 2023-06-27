// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.advance;

import containers.is_iterator;
import containers.iter_difference_t;

import bounded;
import std_module;

namespace containers {

using namespace bounded::literal;

template<typename Iterator, typename Offset>
concept random_access_advancable_by = requires(Iterator & it, Offset offset) {
	it += offset;
};

template<typename Iterator, typename Offset>
using advance_counter = std::common_type_t<
	iter_difference_t<Iterator>,
	std::decay_t<Offset>
>;

export constexpr auto advance(iterator auto & it, auto const offset) {
	for (auto n = advance_counter<decltype(it), decltype(offset)>(0_bi); n != offset; ++n) {
		++it;
	}
}
export constexpr auto advance(bidirectional_iterator auto & it, auto const offset) {
	auto const positive_offset = bounded::abs(bounded::integer(offset));
	for (auto n = advance_counter<decltype(it), decltype(offset)>(0_bi); n != positive_offset; ++n) {
		if (offset >= 0_bi) {
			++it;
		} else {
			--it;
		}
	}
}
export constexpr auto advance(random_access_iterator auto & it, auto const offset) {
	if constexpr (random_access_advancable_by<decltype(it), decltype(offset)>) {
		it += offset;
	} else {
		it += offset.value();
	}
}

template<iterator Iterator>
constexpr auto iterator_one() {
	if constexpr (bounded::bounded_integer<iter_difference_t<Iterator>>) {
		return 1_bi;
	} else {
		return 1;
	}
}

// TODO: propose allowing defaulted auto parameters
export constexpr auto next(iterator auto it, auto const offset) {
	::containers::advance(it, offset);
	return it;
}
export constexpr auto next(iterator auto it) {
	return ::containers::next(std::move(it), iterator_one<decltype(it)>());
}


export constexpr auto prev(bidirectional_iterator auto it, auto const offset) {
	::containers::advance(it, -offset);
	return it;
}
export constexpr auto prev(bidirectional_iterator auto it) {
	return ::containers::prev(std::move(it), iterator_one<decltype(it)>());
}

} // namespace containers
