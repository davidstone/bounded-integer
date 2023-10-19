// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.resize;

import containers.algorithms.erase;
import containers.algorithms.generate;
import containers.append;
import containers.begin_end;
import containers.count_type;
import containers.data;
import containers.forward_random_access_range;
import containers.pop_back;
import containers.range;
import containers.range_value_t;
import containers.repeat_n;
import containers.resizable_container;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

template<typename Container>
constexpr auto resize_impl(Container & container, auto const new_size, auto const generator) {
	auto const container_size = bounded::integer(containers::size(container));
	if (new_size <= container_size) {
		if constexpr (forward_random_access_range<Container &>) {
			auto const it = containers::begin(container) + ::bounded::assume_in_range<containers::count_type<Container>>(new_size);
			containers::erase_to_end(container, it);
		} else {
			while (containers::size(container) != new_size) {
				containers::pop_back(container);
			}
		}
	} else {
		auto const remaining = bounded::increase_min<0>(new_size - container_size, bounded::unchecked);
		::containers::append(container, containers::generate_n(remaining, generator));
	}
}

export template<resizable_container Container>
constexpr auto resize(Container & container, auto const new_size) {
	::containers::resize_impl(container, bounded::integer(new_size), bounded::construct<range_value_t<Container>>);
}
export template<resizable_container Container>
constexpr auto resize(Container & container, auto const new_size, range_value_t<Container> const & value) {
	::containers::resize_impl(container, bounded::integer(new_size), bounded::value_to_function(value));
}

} // namespace containers

using namespace bounded::literal;

template<typename T>
constexpr auto test_resize_empty_to_zero() -> void {
	auto v = T();
	containers::resize(v, 0_bi);
	BOUNDED_ASSERT(v == T());
}

template<typename T>
constexpr auto test_resize_empty_to_one() -> void {
	auto v = T();
	containers::resize(v, 1_bi);
	BOUNDED_ASSERT(v == T({0}));
}

template<typename T>
constexpr auto test_resize_non_empty_to_zero() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 0_bi);
	BOUNDED_ASSERT(v == T());
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_same_size() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 1_bi);
	BOUNDED_ASSERT(v == T({1}));
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_larger() -> void {
	auto v = T({1});
	containers::resize(v, 2_bi);
	BOUNDED_ASSERT(v == T({1, 0}));
}

template<typename T>
constexpr auto test_resize() -> bool {
	test_resize_empty_to_zero<T>();
	test_resize_empty_to_one<T>();
	test_resize_non_empty_to_zero<T>();
	test_resize_non_empty_to_same_size<T>();
	test_resize_non_empty_to_larger<T>();
	return true;
}

static_assert(test_resize<containers::vector<int>>());
static_assert(test_resize<containers::vector<bounded_test::non_copyable_integer>>());
