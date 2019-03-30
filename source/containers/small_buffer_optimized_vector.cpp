// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/small_buffer_optimized_vector.hpp>
#include <containers/string.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

template<auto capacity_, typename T>
void test_generic(bounded::constant_t<capacity_> const capacity, T const & t, std::initializer_list<T> init) {
	using container = containers::small_buffer_optimized_vector<T, 1>;
	auto const default_constructed = container{};
	BOUNDED_TEST(empty(default_constructed));
	
	BOUNDED_TEST(begin(default_constructed) == begin(default_constructed));
	BOUNDED_TEST(begin(default_constructed) == end(default_constructed));
	
	auto const count = container(capacity);
	BOUNDED_TEST(size(count) == capacity);

	for (auto const & value : count) {
		BOUNDED_TEST(value == T{});
	}
	
	auto const count_arg = container(capacity, t);
	BOUNDED_TEST(size(count_arg) == capacity);
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
	
	// TODO: insert(it, it, it) overload
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

auto test_erase() {
	using container = containers::small_buffer_optimized_vector<char, 1>;
	container v = { 1, 2, 3, 4, 5, 6, 7 };
	erase_if(v, [](auto const & value) { return value % 2 == 0; });
	BOUNDED_TEST(v == container({ 1, 3, 5, 7 }));
}

}	// namespace

int main() {
	test_generic(1_bi, '0', { });
	test_generic(1_bi, '0', { '5' });

	test_generic(5_bi, '0', { });
	test_generic(5_bi, '9', { '0', '1', '4' });
	test_generic(5_bi, '-', { '0', '1', '2', '3', '4' });

	test_generic(containers::detail::minimum_small_capacity<char> + 1_bi, '-', {});
	test_generic(containers::detail::minimum_small_capacity<char> + 1_bi, '-', { '0', '1', '2', '3', '4' });
	static_assert(containers::detail::minimum_small_capacity<char> < 30_bi);
	test_generic(30_bi, '-', {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	});

	test_generic(3_bi, containers::string("hi"), { containers::string(""), containers::string("hello"), containers::string(100_bi, '=') });
	
	test_erase();
}
