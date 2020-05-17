// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Iterator, typename Offset>
concept random_access_advancable_by = requires(Iterator & it, Offset offset) {
	it += offset;
};

template<typename Iterator, typename Offset>
using advance_counter = std::common_type_t<
	typename std::iterator_traits<std::decay_t<Iterator>>::difference_type,
	std::decay_t<Offset>
>;

}	// namespace detail

constexpr auto advance(forward_iterator auto & it, auto const offset) {
	for (auto n = detail::advance_counter<decltype(it), decltype(offset)>(0_bi); n != offset; ++n) {
		++it;
	}
}
constexpr auto advance(bidirectional_iterator auto & it, auto const offset) {
	for (auto n = detail::advance_counter<decltype(it), decltype(offset)>(0_bi); n != bounded::abs(offset); ++n) {
		if (offset >= 0_bi) {
			++it;
		} else {
			--it;
		}
	}
}
constexpr auto advance(random_access_iterator auto & it, auto const offset) {
	if constexpr (detail::random_access_advancable_by<decltype(it), decltype(offset)>) {
		it += offset;
	} else {
		it += offset.value();
	}
}

namespace detail {

template<iterator Iterator>
constexpr auto iterator_one() {
	if constexpr (bounded::bounded_integer<typename std::iterator_traits<Iterator>::difference_type>) {
		return 1_bi;
	} else {
		return 1;
	}
}

}	// namespace detail

// TODO: propose allowing defaulted auto parameters
constexpr auto next(iterator auto it, auto const offset) {
	::containers::advance(it, offset);
	return it;
}
constexpr auto next(iterator auto it) {
	return ::containers::next(std::move(it), ::containers::detail::iterator_one<decltype(it)>());
}


constexpr auto prev(iterator auto it, auto const offset) {
	::containers::advance(it, -offset);
	return it;
}
constexpr auto prev(iterator auto it) {
	return ::containers::prev(std::move(it), ::containers::detail::iterator_one<decltype(it)>());
}

}	// namespace containers
