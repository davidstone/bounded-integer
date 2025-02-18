// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.sort.fixed_size_merge_sort;

import containers.algorithms.sort.fixed_size_merge_sort;
import containers.array;
import containers.begin_end;

import bounded;
import std_module;

using namespace bounded::literal;

static_assert([]{
	auto container = containers::array{0, 1};
	containers::fixed_size_merge_sort(containers::begin(container), 1_bi, 1_bi, std::less());
	return container == containers::array{0, 1};
}());

static_assert([]{
	auto container = containers::array{1, 0};
	containers::fixed_size_merge_sort(containers::begin(container), 1_bi, 1_bi, std::less());
	return container == containers::array{0, 1};
}());