// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/dynamic_array.hpp>

#include <containers/algorithms/compare.hpp>

#include <cassert>

namespace {

using namespace bounded::literal;

template<typename T, typename Capacity>
void test_generic(T const & t, Capacity capacity, std::initializer_list<T> init) {
	using container = containers::dynamic_array<T>;
	auto const default_constructed = container{};
	assert(empty(default_constructed));
	
	auto const count = container(capacity);
	assert(size(count) == capacity);

	assert(begin(default_constructed) == begin(default_constructed));
	assert(begin(default_constructed) == end(default_constructed));
	
	for (auto const & value : count) {
		assert(value == T{});
	}
	
	auto const count_arg = container(capacity, t);
	assert(size(count) == capacity);
	for (auto const & value : count_arg) {
		assert(value == t);
	}
	assert(front(count_arg) == t);
	assert(back(count_arg) == t);
	assert(count_arg[0_bi] == t);
	assert(at(count_arg, 0_bi) == t);
	try {
		at(count_arg, static_cast<unsigned>(capacity + 1_bi));
		assert(false);
	} catch (std::out_of_range const &) {
	}

	auto const init_list = container(init);
	assert(containers::equal(init_list, init));
	
	auto copy = init_list;
	assert(containers::equal(copy, init));
	
	auto move = std::move(copy);
	clear(copy);
	assert(move == init_list);
	
	copy = move;
	assert(size(copy) == init.size());
	assert(copy == move);
	
	move = std::move(copy);
	clear(copy);
	assert(empty(copy));
	
	assert(copy == default_constructed);
	
	copy = init;
	assert(copy == init_list);

	assign(copy, init);
	assign(copy, capacity, t);
	
	clear(copy);
	resize(copy, capacity);
	assert(front(copy) == T{});
	assert(size(copy) == capacity);
	resize(copy, 0_bi);
	assert(empty(copy));
	resize(copy, capacity, t);
	assert(copy == count_arg);
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
