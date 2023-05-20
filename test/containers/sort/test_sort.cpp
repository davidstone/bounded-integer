// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.sort.test_sort;

import containers.test.sort.sort_test_data;

import containers.algorithms.all_any_none;
import containers.algorithms.compare;

import containers.is_range;
import containers.range_value_t;
import containers.range_view;
import containers.uninitialized_array;

import std_module;

namespace containers_test {

export constexpr auto test_sort(containers::range auto range, auto function) -> bool {
	return containers::all(range, [&](auto element) {
		function(element.input);
		return element.input == element.expected;
	});
}

export constexpr auto test_sort_relocate(containers::range auto range, auto function) -> bool {
	return containers::all(range, [&]<typename Container>(sort_test_data<Container> element) {
		auto buffer = containers::uninitialized_array<containers::range_value_t<Container>, Container::size()>();
		function(element.input, buffer);
		return containers::equal(containers::range_view(buffer.data(), buffer.size()), element.expected);
	});
}

} // namespace containers_test