// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/flat_map.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array/make_array.hpp>

#include <bounded/value_to_function.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

using mapped_type = containers::map_value_type<bounded_test::integer, bounded_test::integer>;

constexpr auto test_constructor(auto const & input, auto const & expected, auto... maybe_constructor_arg) {
	auto const map = containers::flat_map<bounded_test::integer, bounded_test::integer>(maybe_constructor_arg..., input);
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
	constexpr auto value = [] { return containers::make_array(mapped_type{0, 4}); };
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
	constexpr auto value = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}); };
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
	constexpr auto value = [] { return containers::make_array(mapped_type{1, 2}, mapped_type{0, 4}); };
	constexpr auto expected = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}); };
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
	constexpr auto value = [] { return containers::make_array(mapped_type{1, 4}, mapped_type{1, 2}); };
	constexpr auto expected = [] { return containers::make_array(mapped_type{1, 4}); };
	static_assert(test_constructor(
		value(),
		expected()
	));
	return true;
}
static_assert(test_two_duplicate_keys());

constexpr auto test_three_sorted_unique_keys() {
	constexpr auto value = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}, mapped_type{3, 5}); };
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
	constexpr auto value = [] { return containers::make_array(mapped_type{3, 5}, mapped_type{1, 2}, mapped_type{0, 4}); };
	constexpr auto expected = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}, mapped_type{3, 5}); };
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
	constexpr auto value = [] { return containers::make_array(mapped_type{1, 2}, mapped_type{0, 4}, mapped_type{3, 5}); };
	constexpr auto expected = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}, mapped_type{3, 5}); };
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
	constexpr auto expected = [] { return containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}); };
	static_assert(test_constructor(
		containers::make_array(mapped_type{0, 4}, mapped_type{1, 2}, mapped_type{0, 5}),
		expected()
	));
	static_assert(test_constructor(
		containers::make_array(mapped_type{0, 4}, mapped_type{0, 5}, mapped_type{1, 2}),
		expected()
	));
	return true;
}
static_assert(test_three_keys_with_two_duplicates());

constexpr auto test_three_duplicate_keys() {
	static_assert(test_constructor(
		containers::make_array(mapped_type{0, 4}, mapped_type{0, 2}, mapped_type{0, 5}),
		containers::make_array(mapped_type{0, 4})
	));
	return true;
}
static_assert(test_three_duplicate_keys());

constexpr auto test() {
	using container_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
	auto const init = containers::make_array(mapped_type{1, 2}, mapped_type{2, 5}, mapped_type{3, 3});

	auto container = container_type(init);
	BOUNDED_TEST(containers::equal(container, init));

	container.lazy_insert(5, bounded::value_to_function(3));
	BOUNDED_TEST(containers::equal(container, containers::make_array(mapped_type{1, 2}, mapped_type{2, 5}, mapped_type{3, 3}, mapped_type{5, 3})));

	return true;
}

static_assert(test());

} // namespace
