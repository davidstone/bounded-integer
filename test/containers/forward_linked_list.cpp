// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.forward_linked_list;

import containers.algorithms.advance;
import containers.algorithms.compare;

import containers.test.test_sequence_container;

import containers.array;
import containers.as_mutable_ptr;
import containers.begin_end;
import containers.forward_linked_list;
import containers.front;
import containers.is_empty;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

static_assert(bounded::convertible_to<containers::forward_linked_list<int>::iterator, containers::forward_linked_list<int>::const_iterator>);

static_assert(containers_test::test_sequence_container<containers::forward_linked_list<int>>());
static_assert(containers_test::test_sequence_container<containers::forward_linked_list<bounded_test::non_copyable_integer>>());

template<typename Integer>
constexpr auto test_lazy_insert_after_empty() {
	auto list = containers::forward_linked_list<Integer>();
	auto const it = list.lazy_insert_after(list.before_begin(), bounded::construct<Integer>);
	BOUNDED_ASSERT(containers::equal(list, containers::array{0}));
	BOUNDED_ASSERT(it == containers::begin(list));
}

template<typename Integer>
constexpr auto test_lazy_insert_after_non_empty_before_begin() {
	auto list = containers::forward_linked_list<Integer>({1});
	auto const ptr = std::addressof(containers::front(list));
	auto const it = list.lazy_insert_after(list.before_begin(), bounded::construct<Integer>);
	BOUNDED_ASSERT(containers::equal(list, containers::array{0, 1}));
	BOUNDED_ASSERT(it == containers::begin(list));
	BOUNDED_ASSERT(ptr == std::addressof(*containers::next(containers::begin(list), 1_bi)));
}

template<typename Integer>
constexpr auto test_lazy_insert_after_non_empty_begin() {
	auto list = containers::forward_linked_list<Integer>({1});
	auto const ptr = std::addressof(containers::front(list));
	auto const it = list.lazy_insert_after(containers::begin(list), bounded::construct<Integer>);
	BOUNDED_ASSERT(containers::equal(list, containers::array{1, 0}));
	BOUNDED_ASSERT(it == containers::next(containers::begin(list)));
	BOUNDED_ASSERT(ptr == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_lazy_insert_after() {
	test_lazy_insert_after_empty<Integer>();
	test_lazy_insert_after_non_empty_before_begin<Integer>();
	test_lazy_insert_after_non_empty_begin<Integer>();
	return true;
}

static_assert(test_lazy_insert_after<int>());
static_assert(test_lazy_insert_after<bounded_test::non_copyable_integer>());


template<typename Integer>
constexpr auto test_erase_after_one() {
	auto list = containers::forward_linked_list<Integer>({1});
	auto const it = list.erase_after(list.before_begin());
	BOUNDED_ASSERT(containers::is_empty(list));
	BOUNDED_ASSERT(it == containers::end(list));
}

template<typename Integer>
constexpr auto test_erase_after_two_before_begin() {
	auto list = containers::forward_linked_list<Integer>({1, 2});
	auto const it = list.erase_after(list.before_begin());
	BOUNDED_ASSERT(containers::equal(list, containers::array{2}));
	BOUNDED_ASSERT(it == containers::begin(list));
}

template<typename Integer>
constexpr auto test_erase_after_two_begin() {
	auto list = containers::forward_linked_list<Integer>({1, 2});
	auto const it = list.erase_after(containers::begin(list));
	BOUNDED_ASSERT(containers::equal(list, containers::array{1}));
	BOUNDED_ASSERT(it == containers::end(list));
}

template<typename Integer>
constexpr auto test_erase_after() {
	test_erase_after_one<Integer>();
	test_erase_after_two_before_begin<Integer>();
	test_erase_after_two_begin<Integer>();
	return true;
}

static_assert(test_erase_after<int>());
static_assert(test_erase_after<bounded_test::non_copyable_integer>());


template<typename Integer>
constexpr auto test_splice_after_empty_empty() {
	auto destination = containers::forward_linked_list<Integer>({});
	auto source = containers::forward_linked_list<Integer>({});
	destination.splice_after(destination.before_begin(), source, source.before_begin(), source.before_begin());
	BOUNDED_ASSERT(containers::is_empty(destination));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_one_empty_before() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({});
	destination.splice_after(destination.before_begin(), source, source.before_begin(), source.before_begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_one_empty_after() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({});
	destination.splice_after(containers::begin(destination), source, source.before_begin(), source.before_begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_empty_one() {
	auto destination = containers::forward_linked_list<Integer>({});
	auto source = containers::forward_linked_list<Integer>({2});
	destination.splice_after(destination.before_begin(), source, source.before_begin(), containers::begin(source));
	BOUNDED_ASSERT(containers::equal(destination, containers::array{2}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_one_one_before_all() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({2});
	destination.splice_after(destination.before_begin(), source, source.before_begin(), containers::begin(source));
	BOUNDED_ASSERT(containers::equal(destination, containers::array{2, 1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_one_one_before_none() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({2});
	destination.splice_after(destination.before_begin(), source, source.before_begin(), source.before_begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{2}));
}

template<typename Integer>
constexpr auto test_splice_after_one_one_after_all() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({2});
	destination.splice_after(containers::begin(destination), source, source.before_begin(), containers::begin(source));
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1, 2}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_after_one_one_after_none() {
	auto destination = containers::forward_linked_list<Integer>({1});
	auto source = containers::forward_linked_list<Integer>({2});
	destination.splice_after(containers::begin(destination), source, source.before_begin(), source.before_begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{2}));
}

template<typename Integer>
constexpr auto test_splice_after_middle() {
	auto destination = containers::forward_linked_list<Integer>({1, 2});
	auto source = containers::forward_linked_list<Integer>({3, 4, 5, 6});
	destination.splice_after(containers::begin(destination), source, containers::next(source.before_begin(), 1_bi), containers::next(source.before_begin(), 3_bi));
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1, 4, 5, 2}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{3, 6}));
}

template<typename Integer>
constexpr auto test_splice_after() {
	test_splice_after_empty_empty<Integer>();
	test_splice_after_one_empty_before<Integer>();
	test_splice_after_one_empty_after<Integer>();
	test_splice_after_empty_one<Integer>();
	test_splice_after_one_one_before_all<Integer>();
	test_splice_after_one_one_before_none<Integer>();
	test_splice_after_one_one_after_all<Integer>();
	test_splice_after_one_one_after_none<Integer>();
	test_splice_after_middle<Integer>();
	return true;
}

static_assert(test_splice_after<int>());
static_assert(test_splice_after<bounded_test::non_copyable_integer>());

namespace {

struct recursive {
	containers::forward_linked_list<recursive> m;
};

} // namespace