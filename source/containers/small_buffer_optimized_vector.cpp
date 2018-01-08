// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/small_buffer_optimized_vector.hpp>
#include <containers/string.hpp>

#include <cassert>

namespace {

using namespace bounded::literal;

template<auto capacity_, typename T>
void test_generic(bounded::constant_t<capacity_> const capacity, T const & t, std::initializer_list<T> init) {
	using container = containers::small_buffer_optimized_vector<T, 1>;
	auto const default_constructed = container{};
	assert(empty(default_constructed));
	
	assert(begin(default_constructed) == begin(default_constructed));
	assert(begin(default_constructed) == end(default_constructed));
	
	auto const count = container(capacity);
	assert(size(count) == capacity);

	for (auto const & value : count) {
		assert(value == T{});
		static_cast<void>(value);
	}
	
	auto const count_arg = container(capacity, t);
	assert(size(count_arg) == capacity);
	for (auto const & value : count_arg) {
		assert(value == t);
		static_cast<void>(value);
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
	assert(std::equal(begin(init_list), end(init_list), begin(init), end(init)));
	
	auto copy = init_list;
	assert(std::equal(begin(copy), end(copy), begin(init), end(init)));
	
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

	clear(copy);
	push_back(copy, t);
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	copy.pop_back();
	push_back(copy, T(t));
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	clear(copy);
	insert(copy, begin(copy), t);
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	
	assign(copy, init);
	assign(copy, capacity, t);
	
	// TODO: insert(it, it, it) overload
	auto const old_front = front(copy);
	resize(copy, capacity);
	assert(front(copy) == old_front);
	static_cast<void>(old_front);
	clear(copy);
	resize(copy, capacity);
	assert(front(copy) == T{});
	assert(size(copy) == capacity);
	resize(copy, 0_bi);
	assert(empty(copy));
	resize(copy, capacity, t);
	assert(copy == count_arg);
}

auto test_erase() {
	using container = containers::small_buffer_optimized_vector<char, 1>;
	container v = { 1, 2, 3, 4, 5, 6, 7 };
	erase_if(v, [](auto const & value) { return value % 2 == 0; });
	assert(v == container({ 1, 3, 5, 7 }));
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

	test_generic(3_bi, containers::string("hi"), { containers::string(""), containers::string("hello"), containers::string(100, '=') });
	
	test_erase();
}
