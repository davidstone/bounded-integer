// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/stable_vector.hpp>
#include <containers/array/array.hpp>
#include <containers/emplace_back.hpp>
#include <containers/repeat_n.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

template<std::size_t capacity_, typename T>
constexpr bool test_generic(T const & t, std::initializer_list<T> init) {
	auto const capacity = bounded::constant<capacity_>;
	using container = containers::stable_vector<T, capacity_>;
	auto const default_constructed = container{};
	BOUNDED_TEST(empty(default_constructed));
	static_assert(default_constructed.capacity() == capacity);
	
	auto const count = container(containers::repeat_n(capacity, T{}));
	BOUNDED_TEST(size(count) == capacity);

	BOUNDED_TEST(begin(default_constructed) == begin(default_constructed));
	BOUNDED_TEST(begin(default_constructed) == end(default_constructed));
	
	for (auto const & value : count) {
		BOUNDED_TEST(value == T{});
	}
	
	auto const count_arg = container(containers::repeat_n(capacity, t));
	BOUNDED_TEST(size(count) == capacity);
	for (auto const & value : count_arg) {
		BOUNDED_TEST(value == t);
	}
	BOUNDED_TEST(front(count_arg) == t);
	BOUNDED_TEST(back(count_arg) == t);
	BOUNDED_TEST(count_arg[0_bi] == t);
	BOUNDED_TEST(at(count_arg, 0_bi) == t);

	auto const init_list = container(init);
	BOUNDED_TEST(containers::equal(init_list, init));
	
	auto copy = init_list;
	BOUNDED_TEST(containers::equal(copy, init));
	
	auto move = std::move(copy);
	clear(copy);
	BOUNDED_TEST(move == init_list);
	
	copy = move;
	BOUNDED_TEST(size(copy) == init.size());
	BOUNDED_TEST(copy == move);
	
	move = std::move(copy);
	clear(copy);
	BOUNDED_TEST(empty(copy));
	
	BOUNDED_TEST(copy == default_constructed);
	
	copy = init;
	BOUNDED_TEST(copy == init_list);

	BOUNDED_TEST(data(copy) != data(init_list));
	
	clear(copy);
	push_back(copy, t);
	BOUNDED_TEST(size(copy) == 1_bi);
	BOUNDED_TEST(back(copy) == t);
	containers::pop_back(copy);
	push_back(copy, T(t));
	BOUNDED_TEST(size(copy) == 1_bi);
	BOUNDED_TEST(back(copy) == t);
	clear(copy);
	insert(copy, begin(copy), t);
	BOUNDED_TEST(size(copy) == 1_bi);
	BOUNDED_TEST(back(copy) == t);
	
	assign(copy, init);
	assign(copy, containers::repeat_n(capacity, t));
	
	auto const old_front = front(copy);
	resize(copy, capacity);
	BOUNDED_TEST(front(copy) == old_front);
	clear(copy);
	resize(copy, capacity);
	BOUNDED_TEST(front(copy) == T{});
	BOUNDED_TEST(size(copy) == capacity);
	resize(copy, 0_bi);
	BOUNDED_TEST(empty(copy));
	resize(copy, capacity, t);
	BOUNDED_TEST(copy == count_arg);

	return true;
}

struct non_trivial_non_copyable {
	constexpr non_trivial_non_copyable() {}
	non_trivial_non_copyable(non_trivial_non_copyable const &) = delete;
	non_trivial_non_copyable(non_trivial_non_copyable &&) = default;
	non_trivial_non_copyable & operator=(non_trivial_non_copyable const &) = delete;
	non_trivial_non_copyable & operator=(non_trivial_non_copyable &&) = default;
};

static_assert(test_generic<1>(0, { }));
static_assert(test_generic<1>(0, { 5 }));

static_assert(test_generic<5>(0, { }));
static_assert(test_generic<5>(9, { 0, 1, 4 }));
static_assert(test_generic<5>(-4, { 0, 1, 2, 3, 4 }));
static_assert(test_generic<3>(
	containers::stable_vector<char, 100>(std::string_view("hi")),
	{ containers::stable_vector<char, 100>(), containers::stable_vector<char, 100>(std::string_view("hello")), containers::stable_vector<char, 100>(std::string_view("asdf")) }
));

constexpr bool test_non_trivial_non_copyable() {
	containers::stable_vector<non_trivial_non_copyable, 10> test_no_copies;
	containers::emplace_back(test_no_copies);
	insert(test_no_copies, begin(test_no_copies), non_trivial_non_copyable{});
	return true;
}

static_assert(test_non_trivial_non_copyable());

static_assert(!containers::iterator<containers::stable_vector<std::string, 6>>);
static_assert(containers::is_container<containers::stable_vector<std::string, 6>>);

using index_type = containers::index_type<containers::stable_vector<int, 10>>;
static_assert(bounded::min_value<index_type> == 0_bi);
static_assert(bounded::max_value<index_type> == 9_bi);
static_assert(std::is_same<index_type::overflow_policy, bounded::throw_policy<std::out_of_range>>{});

constexpr bool check_insert_in_middle() {
	containers::stable_vector<int, 10> container = { 1, 2, 3 };
	containers::insert(container, begin(container) + 1_bi, containers::repeat_n(5_bi, 12));
	auto const expected = { 1, 12, 12, 12, 12, 12, 2, 3 };
	return containers::equal(container, expected);
}
static_assert(check_insert_in_middle());

} // namespace
