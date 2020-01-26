// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container, typename Range>
concept member_insertable = requires(Container & container, Range && range) {
	container.insert(
		end(container),
		begin(OPERATORS_FORWARD(range)),
		end(OPERATORS_FORWARD(range))
	);
};

template<typename Container>
concept lazy_push_backable = requires(Container & container, typename Container::value_type (&constructor)()) { lazy_push_back(container, constructor); };

template<typename Container>
concept push_backable = requires(Container & container, typename Container::value_type value_type) { push_back(container, std::move(value_type)); };

namespace common {

// I would like to return an iterator to the start of the appended range, but
// that does not seem possible to do efficiently in general due to potential
// iterator instability.
template<push_backable Container>
constexpr auto append(Container & output, range auto && input) -> void {
	// TODO: Define InputRange and ForwardRange concepts
	using iterator_category = typename std::iterator_traits<decltype(begin(input))>::iterator_category;
	constexpr auto reserve_space = std::is_convertible_v<iterator_category, std::forward_iterator_tag> and reservable<Container>;
	if constexpr (reserve_space) {
		auto const input_size = [&] {
			auto const value = size(input);
			if constexpr (bounded::bounded_integer<decltype(value)>) {
				return value;
			} else {
				return typename Container::size_type(value);
			}
		}();
		auto const offset = linear_size(output);
		if (offset + input_size > output.capacity()) {
			::containers::detail::growth_reallocation(output, input_size);
		}
	}
	if constexpr (appendable_from_capacity<Container> and (!reservable<Container> or reserve_space)) {
		auto const new_end = containers::uninitialized_copy(
			begin(OPERATORS_FORWARD(input)),
			end(OPERATORS_FORWARD(input)),
			end(output)
		);
		output.append_from_capacity(new_end - end(output));
	} else if constexpr (member_insertable<Container, decltype(input)> and std::is_move_constructible_v<typename Container::value_type> and std::is_move_assignable_v<typename Container::value_type>) {
		output.insert(end(output), begin(OPERATORS_FORWARD(input)), end(OPERATORS_FORWARD(input)));
	} else if constexpr (lazy_push_backable<Container>) {
		auto const last = end(OPERATORS_FORWARD(input));
		for (auto it = begin(OPERATORS_FORWARD(input)); it != last; ++it) {
			lazy_push_back(output, [&] { return *it; });
		}
	} else {
		for (decltype(auto) value : OPERATORS_FORWARD(input)) {
			push_back(output, OPERATORS_FORWARD(value));
		}
	}
}

}	// namespace common

using ::containers::detail::common::append;

}	// namespace detail

using ::containers::detail::common::append;

}	// namespace containers
