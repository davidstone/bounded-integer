// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/integer_range.hpp>
#include <containers/is_iterator.hpp>
#include <bounded/integer.hpp>

namespace containers {

template<iterator InputIterator, typename Count, iterator OutputIterator>
constexpr auto copy_n(InputIterator first, Count const count, OutputIterator out) {
	using namespace bounded::literal;
	static_assert(bounded::min_value<Count> > -1_bi, "Negative numbers not supported.");
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (auto const n : integer_range(count)) {
		if constexpr (requires { out + n; first + n; }) {
			*(out + n) = *(first + n);
		} else {
			*out = *first;
			++out;
			++first;
		}
	}
	return result{std::move(first), std::move(out)};
}

}	// namespace containers
