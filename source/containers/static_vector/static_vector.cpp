// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector/static_vector.hpp>
#include <containers/array/array.hpp>
#include <containers/repeat_n.hpp>

#include "../../test_assert.hpp"

namespace {

using namespace bounded::literal;

// This must be a function to work around
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85944
[[maybe_unused]] void constexpr_static_vector() {
	constexpr auto default_constructed = containers::static_vector<int, 9>{};
	static_assert(empty(default_constructed));
	
	constexpr auto default_constructed_elements = containers::static_vector<int, 3>(containers::repeat_default_n<int>(2_bi));
	static_assert(size(default_constructed_elements) == 2_bi);
	static_assert(at(default_constructed_elements, 1_bi) == 0);
	
	constexpr auto copy_constructed = default_constructed_elements;
	static_assert(copy_constructed == default_constructed_elements);
	
	constexpr auto n_copy = containers::static_vector<int, 4>(containers::repeat_n(4_bi, 10));
	static_assert(size(n_copy) == 4_bi);
	static_assert(at(n_copy, 2_bi) == 10);
	static_assert(n_copy == containers::static_vector<int, 4>(containers::repeat_n(4_bi, 10)));
	
	constexpr auto make = []{
		auto value = containers::static_vector<int, 10>{};
		value.emplace_back(5);
		value.emplace_back(15);
		value.emplace_back(20);
		value.emplace(begin(value) + 1_bi, 10);
		value.pop_back();
		auto copy = containers::static_vector<int, 10>{};
		copy = value;
		copy.insert(begin(copy) + 1_bi, value);
		return copy;
	};
	constexpr auto made = make();
	constexpr auto expected = containers::array{5, 5, 10, 15, 10, 15};
	static_assert(containers::equal(begin(made), end(made), begin(expected), end(expected)));
	static_assert(made == made);
	
	constexpr auto bounded_integer_element = containers::static_vector<bounded::integer<0, 10>, 1>(containers::repeat_n(1_bi, 2_bi));
	static_assert(front(bounded_integer_element) == 2_bi);
}

template<std::size_t capacity_, typename T>
void test_generic(T const & t, std::initializer_list<T> init) {
	constexpr auto capacity = bounded::constant<capacity_>;
	using container = containers::static_vector<T, capacity_>;
	auto const default_constructed = container{};
	BOUNDED_TEST(empty(default_constructed));
	static_assert(default_constructed.capacity() == capacity);
	
	auto const count = container(containers::repeat_default_n<T>(capacity));
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
	try {
		at(count_arg, static_cast<unsigned>(capacity + 1_bi));
		BOUNDED_TEST(false);
	} catch (std::out_of_range const &) {
	}

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
	copy.pop_back();
	push_back(copy, T(t));
	BOUNDED_TEST(size(copy) == 1_bi);
	BOUNDED_TEST(back(copy) == t);
	clear(copy);
	insert(copy, begin(copy), t);
	BOUNDED_TEST(size(copy) == 1_bi);
	BOUNDED_TEST(back(copy) == t);
	
	assign(copy, init);
	assign(copy, capacity, t);
	
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
}

struct non_copyable {
	non_copyable() {}
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
	non_copyable & operator=(non_copyable const &) = delete;
	non_copyable & operator=(non_copyable &&) = default;
};

static_assert(std::is_trivially_copy_constructible_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_move_constructible_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_destructible_v<containers::static_vector<int, 3>>);

}	// namespace

int main() {
	test_generic<1>(0, { });
	test_generic<1>(0, { 5 });

	test_generic<5>(0, { });
	test_generic<5>(9, { 0, 1, 4 });
	test_generic<5>(-4, { 0, 1, 2, 3, 4 });

	test_generic<3>(std::string("hi"), { std::string(""), std::string("hello"), std::string(100, '=') });
	
	containers::static_vector<int, 10> container = { 1, 2, 3 };
	using index_type = containers::index_type<decltype(container)>;
	static_assert(index_type::min() == 0_bi);
	static_assert(index_type::max() == 9_bi);
	static_assert(std::is_same<index_type::overflow_policy, bounded::throw_policy<std::out_of_range>>{});
	static_assert(!containers::is_iterator<containers::static_vector<std::string, 6>>);
	static_assert(containers::is_container<containers::static_vector<std::string, 6>>);

	container.insert(begin(container) + 1_bi, containers::repeat_n(5_bi, 12));
	auto const expected = { 1, 12, 12, 12, 12, 12, 2, 3 };
	BOUNDED_TEST(containers::equal(container, expected));
	
	containers::static_vector<non_copyable, 10> test_no_copies;
	test_no_copies.emplace_back();
	insert(test_no_copies, begin(test_no_copies), non_copyable{});
}
