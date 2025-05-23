// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.fixed_size_merge_sort;

import containers.algorithms.sort.sort_exactly_1;
import containers.algorithms.sort.sort_exactly_2;
import containers.algorithms.sort.sort_exactly_3;
import containers.algorithms.sort.sort_exactly_4;
import containers.algorithms.sort.sort_exactly_5;

import containers.algorithms.uninitialized;

import containers.subrange;
import containers.uninitialized_array;

import bounded;
import std_module;

using namespace bounded::literal;
namespace containers {

export constexpr auto fixed_size_merge_sort(auto it, auto const size1, auto const size2, auto const compare) {
	static_assert(size1 <= size2);
	static_assert(size1 > 0_bi);
	static_assert(size2 > 0_bi);
	auto first2 = it + size1;
	auto const last2 = first2 + size2;
	::containers::sort_exactly_n(first2, size2, compare);
	auto buffer = containers::uninitialized_array<std::remove_reference_t<decltype(*it)>, size1>();
	auto first1 = buffer.data();
	auto const last1 = first1 + size1;
	::containers::sort_exactly_n_relocate(it, size1, first1, compare);
	while (true) {
		if (compare(*first2, *first1)) {
			bounded::relocate_at(*it, *first2);
			++first2;
			++it;
			if (first2 == last2) {
				::containers::uninitialized_relocate_no_overlap(subrange(first1, last1), it);
				break;
			}
		} else {
			bounded::relocate_at(*it, *first1);
			++first1;
			if (first1 == last1) {
				break;
			}
			++it;
		}
	}
}

} // namespace containers
