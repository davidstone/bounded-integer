// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/dynamic_array.hpp>

#include <containers/algorithms/compare.hpp>

#include <containers/repeat_n.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

template<typename T>
void test_generic(T const & t, auto capacity, std::initializer_list<T> init) {
	using container = containers::dynamic_array<T>;
	auto const default_constructed = container{};
	BOUNDED_TEST(empty(default_constructed));
	
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
	BOUNDED_TEST(move == init_list);
	
	copy = move;
	BOUNDED_TEST(size(copy) == init.size());
	BOUNDED_TEST(copy == move);
	
	move = std::move(copy);
	BOUNDED_TEST(empty(copy));
	
	BOUNDED_TEST(copy == default_constructed);
	
	copy = init;
	BOUNDED_TEST(copy == init_list);

	assign(copy, init);
	assign(copy, containers::repeat_n(capacity, t));
}

}	// namespace

int main() {
	test_generic(0, 1_bi, { });
	test_generic(0, 1_bi, { 5 });

	test_generic(0, 5_bi, { });
	test_generic(9, 5_bi, { 0, 1, 4 });
	test_generic(-4, 5_bi, { 0, 1, 2, 3, 4 });

	test_generic(std::string("hi"), 3_bi, { std::string(""), std::string("hello"), std::string(100, '=') });
}
