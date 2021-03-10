// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/flat_map.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array/make_array.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

#include <initializer_list>

namespace {

template<typename Container>
constexpr auto map_equals(containers::basic_flat_map<Container> const & lhs, std::initializer_list<typename Container::value_type> const rhs) {
	return containers::equal(lhs, rhs);
}

using mapped_type = containers::map_value_type<bounded::test_int, bounded::test_int>;

constexpr auto test_constructor(auto const & input, auto const & expected, auto... maybe_constructor_arg) {
	auto const map = containers::flat_map<bounded::test_int, bounded::test_int>(maybe_constructor_arg..., input);
	return containers::equal(map, expected);
}

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
constexpr auto test_zero_keys() {
	constexpr auto value = containers::make_array<mapped_type>();
	static_assert(test_constructor(
		value,
		value
	));
	static_assert(test_constructor(
		value,
		value,
		containers::assume_unique
	));
	static_assert(test_constructor(
		value,
		value,
		containers::assume_sorted_unique
	));
	return true;
}
static_assert(test_zero_keys());

constexpr auto test_one_key() {
	constexpr auto value = [] { return containers::make_array(mapped_type(0, 4)); };
	static_assert(test_constructor(
		value(),
		value()
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_unique
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_sorted_unique
	));
	return true;
}
static_assert(test_one_key());

constexpr auto test_two_sorted_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2)); };
	static_assert(test_constructor(
		value(),
		value()
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_unique
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_sorted_unique
	));
	return true;
}
static_assert(test_two_sorted_unique_keys());


constexpr auto test_two_unsorted_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(1, 2), mapped_type(0, 4)); };
	constexpr auto expected = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2)); };
	static_assert(test_constructor(
		value(),
		expected()
	));
	static_assert(test_constructor(
		value(),
		expected(),
		containers::assume_unique
	));
	return true;
}
static_assert(test_two_unsorted_unique_keys());


constexpr auto test_two_duplicate_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(1, 4), mapped_type(1, 2)); };
	constexpr auto expected = [] { return containers::make_array(mapped_type(1, 4)); };
	static_assert(test_constructor(
		value(),
		expected()
	));
	return true;
}
static_assert(test_two_duplicate_keys());

constexpr auto test_three_sorted_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2), mapped_type(3, 5)); };
	static_assert(test_constructor(
		value(),
		value()
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_unique
	));
	static_assert(test_constructor(
		value(),
		value(),
		containers::assume_sorted_unique
	));
	return true;
}
static_assert(test_three_sorted_unique_keys());

constexpr auto test_three_descending_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(3, 5), mapped_type(1, 2), mapped_type(0, 4)); };
	constexpr auto expected = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2), mapped_type(3, 5)); };
	static_assert(test_constructor(
		value(),
		expected()
	));
	static_assert(test_constructor(
		value(),
		expected(),
		containers::assume_unique
	));
	return true;
}
static_assert(test_three_descending_unique_keys());

constexpr auto test_three_shuffled_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type(1, 2), mapped_type(0, 4), mapped_type(3, 5)); };
	constexpr auto expected = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2), mapped_type(3, 5)); };
	static_assert(test_constructor(
		value(),
		expected()
	));
	static_assert(test_constructor(
		value(),
		expected(),
		containers::assume_unique
	));
	return true;
}
static_assert(test_three_shuffled_unique_keys());

constexpr auto test_three_keys_with_two_duplicates() {
	constexpr auto expected = [] { return containers::make_array(mapped_type(0, 4), mapped_type(1, 2)); };
	static_assert(test_constructor(
		containers::make_array(mapped_type(0, 4), mapped_type(1, 2), mapped_type(0, 5)),
		expected()
	));
	static_assert(test_constructor(
		containers::make_array(mapped_type(0, 4), mapped_type(0, 5), mapped_type(1, 2)),
		expected()
	));
	return true;
}
static_assert(test_three_keys_with_two_duplicates());

constexpr auto test_three_duplicate_keys() {
	static_assert(test_constructor(
		containers::make_array(mapped_type(0, 4), mapped_type(0, 2), mapped_type(0, 5)),
		containers::make_array(mapped_type(0, 4))
	));
	return true;
}
static_assert(test_three_duplicate_keys());

constexpr auto test() {
	using container_type = containers::flat_map<bounded::test_int, bounded::test_int>;
	auto const init = std::initializer_list<container_type::value_type>{{1, 2}, {2, 5}, {3, 3}};

	auto container = container_type(init);
	BOUNDED_TEST(map_equals(container, init));

	container.insert(container_type::value_type(4, 4));
	BOUNDED_TEST(map_equals(container, {{1, 2}, {2, 5}, {3, 3}, {4, 4}}));

	container.try_emplace(5, 3);
	BOUNDED_TEST(map_equals(container, {{1, 2}, {2, 5}, {3, 3}, {4, 4}, {5, 3}}));

	container.insert(container_type::value_type(3, 10));
	BOUNDED_TEST(map_equals(container, {{1, 2}, {2, 5}, {3, 3}, {4, 4}, {5, 3}}));

	return true;
}

static_assert(test());

} // namespace
