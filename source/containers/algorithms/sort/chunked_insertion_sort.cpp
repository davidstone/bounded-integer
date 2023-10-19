// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.chunked_insertion_sort;

import containers.algorithms.sort.merge_relocate_second_range;
import containers.algorithms.sort.small_size_optimized_sort;
import containers.algorithms.sort.sort_exactly_1;
import containers.algorithms.sort.sort_exactly_2;
import containers.algorithms.sort.sort_exactly_3;
import containers.algorithms.sort.sort_exactly_4;
import containers.algorithms.sort.sort_exactly_5;
import containers.algorithms.sort.sort_exactly_6;

import containers.begin_end;
import containers.offset_type;
import containers.range;
import containers.range_view;
import containers.size;
import containers.uninitialized_array;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Iterator>
constexpr auto maybe_sort_exactly_n_relocate(Iterator it, auto const size, auto out, auto const compare) -> Iterator {
	constexpr auto max_size = numeric_traits::max_value<offset_type<Iterator>>;
	if constexpr (size <= max_size) {
		return ::containers::sort_exactly_n_relocate(it, size, out, compare);
	} else {
		std::unreachable();
	}
}

template<typename Iterator>
constexpr auto runtime_sort_exactly_n_relocate(Iterator it, auto const size, auto out, auto const compare) -> Iterator {
	auto do_sort = [&](auto const constant_size) {
		return ::containers::maybe_sort_exactly_n_relocate(it, constant_size, out, compare);
	};
	switch (static_cast<std::size_t>(size)) {
		case 0:
			return it;
		case 1:
			return do_sort(1_bi);
		case 2:
			return do_sort(2_bi);
		case 3:
			return do_sort(3_bi);
		case 4:
			return do_sort(4_bi);
		default:
			std::unreachable();
	}
}

export template<typename Compare = std::less<>>
constexpr auto chunked_insertion_sort(range auto && r, Compare const compare = Compare()) -> void {
	constexpr auto chunk_size = 4_bi;
	auto it = containers::begin(r);
	auto const last = containers::end(r);
	#if 1
	auto const initial_sort_size = bounded::min(chunk_size, ::containers::size(r));
	::containers::small_size_optimized_sort(
		range_view(it, initial_sort_size),
		compare,
		[](auto &&, auto) { std::unreachable(); }
	);
	it += initial_sort_size;
	#endif
	auto buffer = containers::uninitialized_array<std::remove_reference_t<decltype(*it)>, chunk_size.value()>();
	while (it != last) {
		auto const count = bounded::min(chunk_size, last - it);
		auto next_it = ::containers::runtime_sort_exactly_n_relocate(it, count, buffer.data(), compare);
		merge_relocate_second_range(
			range_view(containers::begin(r), it),
			range_view(buffer.data(), count),
			next_it,
			compare
		);
		it = next_it;
	}
};

} // namespace containers
