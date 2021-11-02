// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/is_sorted.hpp>
#include <containers/algorithms/unique.hpp>

#include <containers/algorithms/erase.hpp>
#include <containers/append.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/repeat_n.hpp>
#include <containers/vector.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

using Container = containers::vector<bounded_test::integer>;

constexpr void test_unique_copy_less(Container const & source, Container const & expected) {
	auto destination = Container(containers::repeat_n(containers::size(source), 0));
	auto const it = containers::unique_copy_less(begin(source), end(source), begin(destination));
	containers::erase(destination, it, end(destination));
	BOUNDED_TEST(destination == expected);
}

constexpr void test_unique_less(Container source, Container const & expected) {
	BOUNDED_TEST(containers::is_sorted(source));
	BOUNDED_TEST(containers::is_sorted(expected));
	test_unique_copy_less(source, expected);
	test_unique_copy_less(std::move(source), expected);
}

constexpr void test_unique_merge_copy(Container const & lhs, Container const & rhs, Container const & expected) {
	auto result = Container(containers::repeat_n(::bounded::assume_in_range<containers::range_size_t<Container>>(containers::size(lhs) + containers::size(rhs)), 0));
	auto const it = containers::unique_merge_copy(begin(lhs), end(lhs), begin(rhs), end(rhs), begin(result));
	containers::erase(result, it, end(result));

	BOUNDED_TEST(result == expected);
}

constexpr void test_unique_inplace_merge(Container v, Container const & other, Container const & expected) {
	using iterator = containers::iterator_t<Container const &>;
	auto const midpoint = static_cast<containers::iter_difference_t<iterator>>(containers::size(v));
	containers::append(v, other);
	auto const it = containers::unique_inplace_merge(begin(v), begin(v) + midpoint, end(v));
	containers::erase(v, it, end(v));

	BOUNDED_TEST(v == expected);
}

constexpr void test_unique_merge(Container v, Container const & other, Container const & expected) {
	BOUNDED_TEST(containers::is_sorted(v));
	BOUNDED_TEST(containers::is_sorted(other));
	test_unique_merge_copy(v, other, expected);
	test_unique_inplace_merge(std::move(v), other, expected);
}

constexpr bool test_unique() {
	test_unique_less({{1}}, {{1}});
	test_unique_less({{1, 2}}, {{1, 2}});
	test_unique_less({{}}, {{}});
	test_unique_less({{1, 3, 5, 5, 5, 6, 10, 10}}, {{1, 3, 5, 6, 10}});
	test_unique_less({{1, 1, 1, 1, 1, 1, 1, 1}}, {{1}});
	test_unique_merge({{1, 2, 3, 5}}, {{7, 8, 9}}, {{1, 2, 3, 5, 7, 8, 9}});
	test_unique_merge({{1, 3, 5, 7, 9}}, {{2, 2, 2, 3, 3, 6, 7}}, {{1, 2, 3, 5, 6, 7, 9}});
	test_unique_merge({{2}}, {{1}}, {{1, 2}});
	test_unique_merge({{}}, {{6}}, {{6}});
	test_unique_merge({{4}}, {{}}, {{4}});
	test_unique_merge({{}}, {{}}, {{}});
	test_unique_merge({{1}}, {{1}}, {{1}});
	test_unique_merge({{8}}, {{8, 8, 8, 8, 8}}, {{8}});
	// Ideally unique_inplace_merge would not assume the first range has no
	// duplicates, but that is my current use-case. I do not know how to remove
	// this limitation without making the algorithm less efficient.
	// test_unique_merge({{8, 8}}, {{8, 8, 8, 8, 8}}, {{8}});
	return true;
}

static_assert(test_unique());

} // namespace