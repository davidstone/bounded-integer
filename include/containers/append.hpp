// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename, typename = void>
inline constexpr auto has_push_back = false;

template<typename Container>
inline constexpr auto has_push_back<
	Container,
	std::void_t<decltype(push_back(std::declval<Container &>(), std::declval<typename Container::value_type>()))>
> = true;

template<typename, typename = void>
inline constexpr auto has_append_from_capacity = false;

template<typename Container>
inline constexpr auto has_append_from_capacity<
	Container,
	std::void_t<decltype(std::declval<Container &>().append_from_capacity(std::declval<typename Container::size_type>()))>
> = true;

template<typename, typename, typename = void>
inline constexpr auto has_insert = false;

template<typename Container, typename Range>
inline constexpr auto has_insert<
	Container,
	Range,
	std::void_t<decltype(std::declval<Container &>().insert(
		end(std::declval<Container &>()),
		begin(std::declval<Range>()),
		end(std::declval<Range>())
	))>
> = true;

namespace common {

// I would like to return an iterator to the start of the appended range, but
// that does not seem possible to do efficiently in general due to potential
// iterator instability.
template<typename Container, typename Range> requires has_push_back<Container>
constexpr auto append(Container & output, Range && input) -> void {
	// TODO: Define InputRange and ForwardRange concepts
	using iterator_category = typename std::iterator_traits<decltype(begin(input))>::iterator_category;
	constexpr auto reserve_space = std::is_convertible_v<iterator_category, std::forward_iterator_tag> and has_reserve<Container>;
	if constexpr (reserve_space) {
		auto const input_size = [&] {
			auto const value = size(input);
			if constexpr (bounded::is_bounded_integer<decltype(value)>) {
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
	if constexpr (reserve_space and has_append_from_capacity<Container>) {
		auto const new_end = containers::uninitialized_copy(
			begin(BOUNDED_FORWARD(input)),
			end(BOUNDED_FORWARD(input)),
			end(output)
		);
		output.append_from_capacity(new_end - end(output));
	} else if constexpr (has_insert<Container, Range> and std::is_move_constructible_v<typename Container::value_type> and std::is_move_assignable_v<typename Container::value_type>) {
		output.insert(end(output), begin(BOUNDED_FORWARD(input)), end(BOUNDED_FORWARD(input)));
	} else {
		for (decltype(auto) value : BOUNDED_FORWARD(input)) {
			push_back(output, BOUNDED_FORWARD(value));
		}
	}
}

}	// namespace common

using ::containers::detail::common::append;

}	// namespace detail

using ::containers::detail::common::append;

}	// namespace containers
