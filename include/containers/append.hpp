// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {

// I would like to return an iterator to the start of the appended range, but
// that does not seem possible to do efficiently in general due to potential
// iterator instability.
template<detail::push_backable Container>
constexpr auto append(Container & output, range auto && input) -> void {
	// TODO: Define InputRange and ForwardRange concepts
	using iterator_category = typename std::iterator_traits<decltype(containers::begin(input))>::iterator_category;
	constexpr auto reserve_space = std::is_convertible_v<iterator_category, std::forward_iterator_tag> and detail::reservable<Container>;
	if constexpr (reserve_space) {
		auto const input_size = [&] {
			auto const value = containers::size(input);
			if constexpr (bounded::bounded_integer<decltype(value)>) {
				return value;
			} else {
				return typename Container::size_type(value);
			}
		}();
		auto const offset = ::containers::detail::linear_size(output);
		if (offset + input_size > output.capacity()) {
			::containers::detail::growth_reallocation(output, input_size);
		}
	}
	if constexpr (detail::appendable_from_capacity<Container> and (!detail::reservable<Container> or reserve_space)) {
		auto const new_end = containers::uninitialized_copy(OPERATORS_FORWARD(input), containers::end(output));
		output.append_from_capacity(new_end - containers::end(output));
	} else if constexpr (detail::lazy_push_backable<Container>) {
		::containers::detail::lazy_push_back_range(output, OPERATORS_FORWARD(input));
	} else {
		::containers::detail::push_back_range(output, OPERATORS_FORWARD(input));
	}
}

} // namespace containers
