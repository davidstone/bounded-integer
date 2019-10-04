// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/vector.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

template<std::size_t capacity_, typename T>
void test_generic(T const & t, std::initializer_list<T> init) {
	constexpr auto capacity = bounded::constant<capacity_>;
	using container = containers::vector<T>;
	auto const default_constructed = container{};
	BOUNDED_TEST(empty(default_constructed));
	BOUNDED_TEST(default_constructed.capacity() == 0_bi);
	
	BOUNDED_TEST(begin(default_constructed) == begin(default_constructed));
	BOUNDED_TEST(begin(default_constructed) == end(default_constructed));
	
	auto const count = container(containers::repeat_default_n<T>(capacity));
	BOUNDED_TEST(size(count) == capacity);

	for (auto const & value : count) {
		BOUNDED_TEST(value == T{});
	}
	
	auto const count_arg = container(containers::repeat_n(capacity, t));
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
	using container = containers::vector<int>;
	container v = { 1, 2, 3, 4, 5, 6, 7 };
	erase_if(v, [](auto const & value) { return value % 2 == 0; });
	BOUNDED_TEST(v == container({ 1, 3, 5, 7 }));
}

auto test_reserve() {
	using container = containers::vector<int>;
	auto v = container{};
	BOUNDED_TEST(v.capacity() == 0_bi);
	v.reserve(10_bi);
	BOUNDED_TEST(v.capacity() == 10_bi);
	v.shrink_to_fit();
	BOUNDED_TEST(v.capacity() == 0_bi);
}

struct complex_resource {
	complex_resource() = default;
	explicit complex_resource(bounded::integral auto const size):
		data(containers::repeat_default_n<int>(size))
	{
	}

	complex_resource(complex_resource const & other) = default;
	complex_resource & operator=(complex_resource const & other) = default;

	containers::vector<int> data;
};

constexpr auto operator==(complex_resource const & lhs, complex_resource const & rhs) {
	return lhs.data == rhs.data;
}

BOUNDED_COMPARISON

}	// namespace

int main() {
	test_generic<1>(0, { });
	test_generic<1>(0, { 5 });

	test_generic<5>(0, { });
	test_generic<5>(9, { 0, 1, 4 });
	test_generic<5>(-4, { 0, 1, 2, 3, 4 });

	test_generic<3>(std::string("hi"), { std::string(""), std::string("hello"), std::string(100, '=') });
	test_generic<6>(complex_resource(5_bi), { complex_resource(1_bi), complex_resource(0_bi), complex_resource(10_bi), complex_resource(10_bi) });
	
	test_erase();
	test_reserve();
}
