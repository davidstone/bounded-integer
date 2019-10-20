// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/find.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

template<iterator ForwardIterator>
constexpr auto remove_if(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto predicate) {
	auto new_last = ::containers::find_if(first, last, predicate);
	if (new_last != last) {
		for (auto it = ::containers::next(new_last); it != last; ++it) {
			if (!predicate(*it)) {
				*new_last = std::move(*it);
				++new_last;
			}
		}
	}
	return new_last;
}

constexpr auto remove_if(range auto & source, auto predicate) {
	return ::containers::remove_if(begin(source), end(source), BOUNDED_FORWARD(predicate));
}


template<iterator ForwardIterator>
constexpr auto remove(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto const & value) {
	return ::containers::remove_if(first, last, bounded::equal_to(value));
}

constexpr auto remove(range auto & source, auto const & value) {
	return ::containers::remove_if(begin(source), end(source), value);
}

}	// namespace containers
