// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.copy;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.reverse_iterator;
import containers.array;
import containers.begin_end;
import containers.dereference;
import containers.is_iterator;
import containers.is_range;
import containers.range_view;

import bounded;
import std_module;

export import containers.common_iterator_functions;

namespace containers {

template<typename Input, typename Output>
struct copy_result {
	Input input;
	Output output;
};

template<typename Input, typename Output>
copy_result(Input, Output) -> copy_result<Input, Output>;

export template<range Input>
constexpr auto copy(Input && input, iterator auto output) {
	auto first_it = copy_or_relocate_from(OPERATORS_FORWARD(input), [&](auto make) {
		*output = make();
		++output;
	});
	return copy_result{std::move(first_it), std::move(output)};
}

export template<range Input>
constexpr auto copy(Input && input, range auto && output) {
	// TODO: Transform this into a relocate when appropriate
	auto in_first = containers::begin(OPERATORS_FORWARD(input));
	auto const in_last = containers::end(OPERATORS_FORWARD(input));
	auto out_first = containers::begin(OPERATORS_FORWARD(output));
	auto const out_last = containers::end(OPERATORS_FORWARD(output));
	for (; in_first != in_last and out_first != out_last; ++in_first, ++out_first) {
		*out_first = dereference<Input>(in_first);
	}
	return copy_result{std::move(in_first), std::move(out_first)};
}

export constexpr auto copy_backward(range auto && input, iterator auto out_last) {
	return ::containers::copy(
		containers::reversed(OPERATORS_FORWARD(input)),
		containers::reverse_iterator(std::move(out_last))
	).output.base();
}

} // namespace containers

using namespace bounded::literal;

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy(
		containers::range_view(containers::begin(container), containers::begin(container)),
		containers::begin(container) + 2_bi
	);
	return container == containers::array{1, 2, 3, 4};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::begin(container) + 2_bi
	);
	return container == containers::array{1, 2, 1, 2};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3, 4};
	containers::copy_backward(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::end(container)
	);
	return container == containers::array{1, 2, 1, 2};
}());

static_assert([] {
	auto container = containers::array{1, 2, 3};
	containers::copy_backward(
		containers::range_view(containers::begin(container), containers::begin(container) + 2_bi),
		containers::end(container)
	);
	return container == containers::array{1, 1, 2};
}());
