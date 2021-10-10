// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/dereference.hpp>

namespace containers {

constexpr auto copy(range auto && input, iterator auto output) {
	auto first = containers::begin(OPERATORS_FORWARD(input));
	auto const last = containers::end(OPERATORS_FORWARD(input));
	for (; first != last; ++first, ++output) {
		*output = dereference<decltype(input)>(first);
	}
	struct result {
		decltype(first) input;
		decltype(output) output;
	};
	return result{std::move(first), std::move(output)};
}

constexpr auto copy(range auto && input, range auto && output) {
	auto in_first = containers::begin(OPERATORS_FORWARD(input));
	auto const in_last = containers::end(OPERATORS_FORWARD(input));
	auto out_first = containers::begin(OPERATORS_FORWARD(output));
	auto const out_last = containers::end(OPERATORS_FORWARD(output));
	for (; in_first != in_last and out_first != out_last; ++in_first, ++out_first) {
		*out_first = dereference<decltype(input)>(in_first);
	}
	struct result {
		decltype(in_first) input;
		decltype(out_first) output;
	};
	return result{std::move(in_first), std::move(out_first)};
}

constexpr auto copy_backward(range auto && input, iterator auto out_last) {
	return ::containers::copy(
		containers::reversed(OPERATORS_FORWARD(input)),
		containers::reverse_iterator(std::move(out_last))
	).output.base();
}

} // namespace containers
