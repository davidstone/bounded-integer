// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.common_prefix;

import containers.algorithms.advance;
import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.front_back;
import containers.is_empty;
import containers.iter_difference_t;
import containers.range_view;
import containers.to_radix_sort_key;

import bounded;
import std_module;

namespace containers {

template<typename T>
concept view = is_range_view<T>;

export constexpr auto common_prefix(
	view auto to_sort,
	auto const & extract_key,
	auto && element_key,
	iter_difference_t<decltype(containers::begin(extract_key(containers::front(to_sort))))> const start_index
) -> std::size_t {
	if (containers::is_empty(to_sort)) {
		return 0;
	}
	auto const first = containers::begin(to_sort);
	auto const last = containers::end(to_sort);
	auto const & first_range = extract_key(*first);
	auto largest_match = containers::end(first_range) - (containers::begin(first_range) + start_index);
	for (auto it = containers::next(first); it != last; ++it) {
		auto const & current_range = extract_key(*it);
		largest_match = bounded::min(
			largest_match,
			std::mismatch(
				containers::begin(first_range) + start_index,
				containers::end(first_range),
				containers::begin(current_range) + start_index,
				containers::end(current_range),
				[&](auto const & lhs, auto const & rhs) {
					return element_key(lhs) == element_key(rhs);
				}
			).first - (containers::begin(first_range) + start_index)
		);
	}
	return static_cast<std::size_t>(largest_match);
}

} // namespace containers

using namespace bounded::literal;

template<typename T, std::size_t size>
constexpr auto test_common_prefix(containers::c_array<T, size> const & source, auto const start_index, std::size_t const expected) {
	auto const prefix = ::containers::common_prefix(
		containers::range_view(source),
		containers::to_radix_sort_key,
		containers::to_radix_sort_key,
		start_index
	);
	return prefix == expected;
}
static_assert(test_common_prefix<containers::array<std::uint8_t, 1_bi>>(
	{{1}, {1}},
	0_bi,
	1U
));
static_assert(test_common_prefix<containers::array<std::uint8_t, 1_bi>>(
	{{1}, {1}},
	1_bi,
	0U
));
