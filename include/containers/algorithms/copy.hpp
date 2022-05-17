// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy_or_relocate_from.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/dereference.hpp>

namespace containers {
namespace detail {

template<typename Input, typename Output>
struct copy_result {
	Input input;
	Output output;
};

template<typename Input, typename Output>
copy_result(Input, Output) -> copy_result<Input, Output>;

} // namespace detail

template<range Input>
constexpr auto copy(Input && input, iterator auto output) {
	auto first_it = copy_or_relocate_from(OPERATORS_FORWARD(input), [&](auto make) {
		*output = make();
		++output;
	});
	return detail::copy_result{std::move(first_it), std::move(output)};
}

template<range Input>
constexpr auto copy(Input && input, range auto && output) {
	// TODO: Transform this into a relocate when appropriate
	auto in_first = containers::begin(OPERATORS_FORWARD(input));
	auto const in_last = containers::end(OPERATORS_FORWARD(input));
	auto out_first = containers::begin(OPERATORS_FORWARD(output));
	auto const out_last = containers::end(OPERATORS_FORWARD(output));
	for (; in_first != in_last and out_first != out_last; ++in_first, ++out_first) {
		*out_first = dereference<Input>(in_first);
	}
	return detail::copy_result{std::move(in_first), std::move(out_first)};
}

constexpr auto copy_backward(range auto && input, iterator auto out_last) {
	return ::containers::copy(
		containers::reversed(OPERATORS_FORWARD(input)),
		containers::reverse_iterator(std::move(out_last))
	).output.base();
}

} // namespace containers
