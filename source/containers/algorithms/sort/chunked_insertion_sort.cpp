// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.chunked_insertion_sort;

import containers.algorithms.sort.merge_relocate_second_range;
import containers.algorithms.sort.small_size_optimized_sort;
import containers.algorithms.sort.sort_exactly_n;

import containers.begin_end;
import containers.integer_range;
import containers.offset_type;
import containers.range;
import containers.size;
import containers.subrange;
import containers.uninitialized_array;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

struct chunked_insertion_sort_t {
	template<typename Compare = std::less<>>
	static constexpr auto operator()(range auto && r, Compare const compare = Compare()) -> void {
		constexpr auto chunk_size = 4_bi;
		auto const size = ::containers::size(r);
		auto it = containers::begin(r);
		auto const initial_sort_size = size % chunk_size;
		::containers::small_size_optimized_sort(
			subrange(it, initial_sort_size),
			compare,
			[](auto &&, auto) { std::unreachable(); }
		);
		it += initial_sort_size;
		auto buffer = containers::uninitialized_array<std::remove_reference_t<decltype(*it)>, chunk_size>();
		for (auto _ : containers::integer_range(size / chunk_size)) {
			auto const next_it = ::containers::sort_exactly_n_relocate(it, chunk_size, buffer.data(), compare);
			merge_relocate_second_range(
				subrange(containers::begin(r), it),
				buffer,
				next_it,
				compare
			);
			it = next_it;
		}
		BOUNDED_ASSERT(it == containers::end(r));
	}
};
export constexpr auto chunked_insertion_sort = chunked_insertion_sort_t();

} // namespace containers
