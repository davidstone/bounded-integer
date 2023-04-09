// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.forward_linked_list;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.destroy_range;
import containers.array;
import containers.assign;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.clear;
import containers.common_functions;
import containers.compare_container;
import containers.contiguous_iterator;
import containers.front_back;
import containers.initializer_range;
import containers.is_empty;
import containers.linked_list_helper;
import containers.range_value_t;
import containers.test_sequence_container;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

struct forward_link {
	forward_link() = default;
	forward_link(forward_link &&) = delete;
	forward_link(forward_link const &) = delete;
	auto operator=(forward_link &&) = delete;
	auto operator=(forward_link const &) = delete;

	forward_link * next = nullptr;
};

export template<typename T>
struct forward_linked_list : private lexicographical_comparison::base {
	using const_iterator = list_iterator<forward_linked_list, forward_link const, T>;
	using iterator = list_iterator<forward_linked_list, forward_link, T>;

	constexpr forward_linked_list() = default;

	constexpr explicit forward_linked_list(constructor_initializer_range<forward_linked_list> auto && source) {
		::containers::assign_to_empty(*this, OPERATORS_FORWARD(source));
	}
	
	template<std::size_t source_size>
	constexpr forward_linked_list(c_array<T, source_size> && source) {
		::containers::assign_to_empty(*this, std::move(source));
	}
	template<std::same_as<empty_c_array_parameter> Source = empty_c_array_parameter>
	constexpr forward_linked_list(Source) {
	}

	constexpr forward_linked_list(forward_linked_list && other) noexcept {
		swap(*this, other);
	}

	constexpr forward_linked_list(forward_linked_list const & other) {
		::containers::assign_to_empty(*this, other);
	}

	constexpr ~forward_linked_list() noexcept {
		::containers::clear(*this);
	}

	constexpr auto operator=(forward_linked_list && other) & noexcept -> forward_linked_list & {
		swap(*this, other);
		return *this;
	}
	constexpr auto operator=(forward_linked_list const & other) & -> forward_linked_list & {
		if (this != std::addressof(other)) {
			containers::assign(*this, other);
		}
		return *this;
	}

	friend constexpr auto swap(forward_linked_list & lhs, forward_linked_list & rhs) noexcept -> void {
		std::swap(lhs.m_sentinel.next, rhs.m_sentinel.next);
	}

	constexpr auto before_begin() const -> const_iterator {
		return const_iterator(std::addressof(m_sentinel));
	}
	constexpr auto before_begin() -> iterator {
		return iterator(std::addressof(m_sentinel));
	}
	constexpr auto end() const -> const_iterator {
		return const_iterator(nullptr);
	}
	constexpr auto end() -> iterator {
		return iterator(nullptr);
	}

	constexpr auto mutable_iterator(const_iterator const it) & -> iterator {
		return iterator(as_mutable(it.m_links));
	}

	constexpr auto lazy_insert_after(const_iterator const before, bounded::construct_function_for<T> auto && constructor) & -> iterator {
		BOUNDED_ASSERT(before != end());
		auto ptr = containers::make_node<node_t>(OPERATORS_FORWARD(constructor));
		auto const mutable_before = mutable_iterator(before);
		ptr->next = mutable_before.m_links->next;
		mutable_before.m_links->next = ptr;
		return iterator(ptr);
	}
	constexpr auto erase_after(const_iterator const before) & -> iterator {
		BOUNDED_ASSERT(before != end());
		auto const mutable_before = mutable_iterator(before);
		auto const it = containers::next(mutable_before);
		auto const after = containers::next(it);
		mutable_before.m_links->next = after.m_links;
		auto const ptr = static_cast<node_t *>(it.m_links);
		destroy_node(ptr);
		return after;
	}

	// Unlike `std::forward_list`, the final iterator is before_last, not last.
	// This allows splice_after to operate in constant time.
	constexpr auto splice_after(const_iterator const before, forward_linked_list & other, const_iterator const before_first, const_iterator const before_last) & -> void {
		BOUNDED_ASSERT(this != std::addressof(other));
		BOUNDED_ASSERT(before_last != other.end());
		if (before_first == before_last) {
			return;
		}
		auto const mutable_before = other.mutable_iterator(before);
		auto const mutable_before_first = other.mutable_iterator(before_first);
		auto const mutable_before_last = other.mutable_iterator(before_last);
		auto const original_next = std::exchange(mutable_before.m_links->next, mutable_before_first.m_links->next);
		mutable_before_first.m_links->next = std::exchange(mutable_before_last.m_links->next, original_next);
	}

private:
	using node_t = linked_list_node<forward_link, T>;
	forward_link m_sentinel;
};

template<typename Range>
forward_linked_list(Range &&) -> forward_linked_list<std::decay_t<range_value_t<Range>>>;

} // namespace containers

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