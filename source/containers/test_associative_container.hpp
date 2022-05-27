// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "test_sequence_container.hpp"

#include <containers/algorithms/all_any_none.hpp>
#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>
#include <containers/begin_end.hpp>
#include <containers/iterator_t.hpp>
#include <containers/range_value_t.hpp>

#include <bounded/value_to_function.hpp>

namespace containers_test {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_associative_container_concepts() -> void {
	static_assert(containers::associative_range<Container const &>);
	static_assert(containers::associative_range<Container &>);
	static_assert(containers::associative_range<Container &&>);
	static_assert(containers::associative_range<Container>);
	static_assert(containers::associative_container<Container>);
}

template<typename Container>
constexpr auto make_zero_keys() {
	return containers::to_array<containers::range_value_t<Container>>({});
}
template<typename Container>
constexpr auto test_zero_keys() -> void {
	constexpr auto make = [] { return make_zero_keys<Container>(); };
	static_assert(containers::equal(
		Container(make()),
		make()
	));
	static_assert(containers::equal(
		Container(containers::assume_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto make_one_key() {
	return containers::to_array<containers::range_value_t<Container>>({
		{0, 4},
	});
}
template<typename Container>
constexpr auto test_one_key() -> void {
	constexpr auto make = [] { return make_one_key<Container>(); };
	static_assert(containers::equal(
		Container(make()),
		make()
	));
	static_assert(containers::equal(
		Container(containers::assume_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto make_two_sorted_unique_keys() {
	return containers::to_array<containers::range_value_t<Container>>({
		{0, 4},
		{1, 2},
	});
}
template<typename Container>
constexpr auto test_two_sorted_unique_keys() -> void {
	constexpr auto make = [] { return make_two_sorted_unique_keys<Container>(); };
	static_assert(containers::equal(
		Container(make()),
		make()
	));
	static_assert(containers::equal(
		Container(containers::assume_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto make_three_sorted_unique_keys() {
	return containers::to_array<containers::range_value_t<Container>>({
		{0, 4},
		{1, 2},
		{3, 5},
	});
}
template<typename Container>
constexpr auto test_three_sorted_unique_keys() -> void {
	constexpr auto make = [] { return make_three_sorted_unique_keys<Container>(); };
	static_assert(containers::equal(
		Container(make()),
		make()
	));
	static_assert(containers::equal(
		Container(containers::assume_unique, make()),
		make()
	));
}

constexpr auto map_contains_exactly(auto const & map, auto const & values) -> bool {
	BOUNDED_TEST(containers::size(map) == containers::size(values));
	return containers::all(values, [&](auto const & value) {
		auto const it = map.find(value.key);
		return it != containers::end(map) and *it == value;
	});
}

template<typename Container>
constexpr auto make_two_unsorted_unique_keys() {
	return containers::to_array<containers::range_value_t<Container>>({
		{1, 2},
		{0, 4},
	});
}
template<typename Container>
constexpr auto test_two_unsorted_unique_keys() -> void {
	constexpr auto make = [] { return make_two_unsorted_unique_keys<Container>(); };
	static_assert(map_contains_exactly(Container(make()), make()));
	static_assert(map_contains_exactly(Container(containers::assume_unique, make()), make()));
}

template<typename Container>
constexpr auto test_two_duplicate_keys() -> void {
	using value_type = containers::range_value_t<Container>;
	constexpr auto make = [] {
		return containers::to_array<value_type>({
			{1, 4},
			{1, 2},
		});
	};
	constexpr auto expected = [] {
		return containers::to_array<value_type>({
			{1, 4}
		});
	};
	static_assert(map_contains_exactly(Container(make()), expected()));
}

template<typename Container>
constexpr auto test_three_keys_with_two_duplicates() -> void {
	using value_type = containers::range_value_t<Container>;
	constexpr auto expected_first = [] {
		return containers::to_array<value_type>({
			{0, 4},
			{1, 2}
		});
	};
	constexpr auto expected_second = [] {
		return containers::to_array<value_type>({
			{0, 5},
			{1, 2}
		});
	};
	constexpr auto check = [=](containers::c_array<value_type, 3> && source) {
		auto const container = Container(std::move(source));
		return
			map_contains_exactly(container, expected_first()) or
			map_contains_exactly(container, expected_second());
	};
	static_assert(check({
		{0, 4},
		{0, 5},
		{1, 2},
	}));
	static_assert(check({
		{0, 4},
		{1, 2},
		{0, 5},
	}));
	static_assert(check({
		{1, 2},
		{0, 4},
		{0, 5},
	}));
}

template<typename Container>
constexpr auto test_three_duplicate_keys() -> void {
	using value_type = containers::range_value_t<Container>;
	static_assert(
		map_contains_exactly(
			Container(containers::to_array<value_type>({
				{0, 4},
				{0, 2},
				{0, 5}
			})),
			containers::to_array<value_type>({
				{0, 4}
			})
		) or
		map_contains_exactly(
			Container(containers::to_array<value_type>({
				{0, 4},
				{0, 2},
				{0, 5}
			})),
			containers::to_array<value_type>({
				{0, 2}
			})
		) or
		map_contains_exactly(
			Container(containers::to_array<value_type>({
				{0, 4},
				{0, 2},
				{0, 5}
			})),
			containers::to_array<value_type>({
				{0, 5}
			})
		)
	);
}

template<typename Container>
constexpr auto test_associative_container_constructors() -> bool {
	test_sequence_container_default_constructed_empty<Container>();
	test_zero_keys<Container>();
	test_one_key<Container>();
	test_two_sorted_unique_keys<Container>();
	test_two_unsorted_unique_keys<Container>();
	test_two_duplicate_keys<Container>();
	test_three_sorted_unique_keys<Container>();
	test_three_keys_with_two_duplicates<Container>();
	test_three_duplicate_keys<Container>();
	return true;
}

template<typename Container>
constexpr auto test_lazy_insert_new_key() {
	using value_type = containers::range_value_t<Container>;
	auto container = Container(containers::to_array<value_type>({
		{1, 2},
		{2, 5},
		{3, 3}
	}));
	container.lazy_insert(5, bounded::value_to_function(3));
	BOUNDED_TEST(containers::equal(
		container,
		containers::to_array<value_type>({
			{1, 2},
			{2, 5},
			{3, 3},
			{5, 3}
		})
	));

	return true;
}

template<typename Container>
constexpr auto test_lazy_insert_existing_key() {
	using value_type = containers::range_value_t<Container>;
	auto container = Container(containers::to_array<value_type>({
		{1, 2},
		{2, 5},
		{3, 3}
	}));
	container.lazy_insert(2, bounded::value_to_function(3));
	BOUNDED_TEST(containers::equal(
		container,
		containers::to_array<value_type>({
			{1, 2},
			{2, 5},
			{3, 3}
		})
	));

	return true;
}

template<typename Container>
constexpr auto test_associative_container() -> bool {
	test_associative_container_concepts<Container>();
	test_range_based_for_loop<Container>();

	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({});
	});
	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({{5, 15}});
	});
	test_sequence_container_from<Container>([] {
		return containers::to_array<containers::range_value_t<Container>>({
			{0, 10},
			{1, 11},
			{4, 14}
		});
	});

	test_associative_container_constructors<Container>();
	test_lazy_insert_new_key<Container>();
	test_lazy_insert_existing_key<Container>();

	return true;
}

} // namespace containers_test