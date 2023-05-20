// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.bidirectional_linked_list;

import containers.algorithms.advance;
import containers.algorithms.compare;

import containers.test.test_sequence_container;

import containers.array;
import containers.begin_end;
import containers.bidirectional_linked_list;
import containers.front_back;
import containers.is_empty;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

static_assert(bounded::convertible_to<containers::bidirectional_linked_list<int>::iterator, containers::bidirectional_linked_list<int>::const_iterator>);

static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<int>>());
static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<bounded_test::integer>>());

template<typename Integer>
constexpr auto test_lazy_push_back_empty() {
	auto list = containers::bidirectional_linked_list<Integer>();
	auto const & element = list.lazy_push_back(bounded::construct<Integer>);
	BOUNDED_ASSERT(containers::equal(list, containers::array{0}));
	BOUNDED_ASSERT(std::addressof(element) == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_lazy_push_back_non_empty() {
	auto list = containers::bidirectional_linked_list<Integer>({1});
	auto const ptr = std::addressof(containers::front(list));
	auto const & element = list.lazy_push_back(bounded::construct<Integer>);
	BOUNDED_ASSERT(containers::equal(list, containers::array{1, 0}));
	BOUNDED_ASSERT(std::addressof(element) == std::addressof(containers::back(list)));
	BOUNDED_ASSERT(ptr == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_lazy_push_back() {
	test_lazy_push_back_empty<Integer>();
	test_lazy_push_back_non_empty<Integer>();
	return true;
}

static_assert(test_lazy_push_back<int>());
static_assert(test_lazy_push_back<bounded_test::non_copyable_integer>());


template<typename Integer>
constexpr auto test_pop_back_one() {
	auto list = containers::bidirectional_linked_list<Integer>({1});
	list.pop_back();
	BOUNDED_ASSERT(containers::is_empty(list));
}

template<typename Integer>
constexpr auto test_pop_back_two() {
	auto list = containers::bidirectional_linked_list<Integer>({1, 2});
	auto const ptr = std::addressof(containers::front(list));
	list.pop_back();
	BOUNDED_ASSERT(containers::equal(list, containers::array{1}));
	BOUNDED_ASSERT(ptr == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_pop_back() {
	test_pop_back_one<Integer>();
	test_pop_back_two<Integer>();
	return true;
}

static_assert(test_pop_back<int>());
static_assert(test_pop_back<bounded_test::non_copyable_integer>());


template<typename Integer>
constexpr auto test_splice_empty_empty() {
	auto destination = containers::bidirectional_linked_list<Integer>({});
	auto source = containers::bidirectional_linked_list<Integer>({});
	destination.splice(destination.begin(), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::is_empty(destination));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_one_empty_before() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({});
	destination.splice(destination.begin(), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_one_empty_after() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({});
	destination.splice(destination.end(), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_empty_one() {
	auto destination = containers::bidirectional_linked_list<Integer>({});
	auto source = containers::bidirectional_linked_list<Integer>({2});
	destination.splice(destination.begin(), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{2}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_one_one_before_all() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({2});
	destination.splice(destination.begin(), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{2, 1}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_one_one_before_none() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({2});
	destination.splice(destination.begin(), source, source.begin(), source.begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{2}));
}

template<typename Integer>
constexpr auto test_splice_one_one_after_all() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({2});
	destination.splice(containers::end(destination), source, source.begin(), source.end());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1, 2}));
	BOUNDED_ASSERT(containers::is_empty(source));
}

template<typename Integer>
constexpr auto test_splice_one_one_after_none() {
	auto destination = containers::bidirectional_linked_list<Integer>({1});
	auto source = containers::bidirectional_linked_list<Integer>({2});
	destination.splice(containers::end(destination), source, source.begin(), source.begin());
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{2}));
}

template<typename Integer>
constexpr auto test_splice_middle() {
	auto destination = containers::bidirectional_linked_list<Integer>({1, 2});
	auto source = containers::bidirectional_linked_list<Integer>({3, 4, 5, 6});
	destination.splice(containers::next(containers::begin(destination)), source, containers::next(source.begin(), 1_bi), containers::next(source.begin(), 3_bi));
	BOUNDED_ASSERT(containers::equal(destination, containers::array{1, 4, 5, 2}));
	BOUNDED_ASSERT(containers::equal(source, containers::array{3, 6}));
}

template<typename Integer>
constexpr auto test_splice() {
	test_splice_empty_empty<Integer>();
	test_splice_one_empty_before<Integer>();
	test_splice_one_empty_after<Integer>();
	test_splice_empty_one<Integer>();
	test_splice_one_one_before_all<Integer>();
	test_splice_one_one_before_none<Integer>();
	test_splice_one_one_after_all<Integer>();
	test_splice_one_one_after_none<Integer>();
	test_splice_middle<Integer>();
	return true;
}

static_assert(test_splice<int>());
static_assert(test_splice<bounded_test::non_copyable_integer>());

namespace {

struct recursive {
	containers::bidirectional_linked_list<recursive> m;
};

} // namespace