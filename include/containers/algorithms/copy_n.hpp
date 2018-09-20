// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/integer_range.hpp>
#include <bounded/integer.hpp>

namespace containers {

template<typename InputIterator, typename Size, typename OutputIterator>
constexpr auto copy_n(InputIterator first, Size const count, OutputIterator out) {
	using namespace bounded::literal;
	static_assert(count > -1_bi, "Negative numbers not supported.");
	struct result {
		InputIterator input;
		OutputIterator output;
	};
	for (auto const n [[maybe_unused]] : integer_range(count)) {
		*out = *first;
		++out;
		++first;
	}
	return result { first, out };
}

}	// namespace containers
