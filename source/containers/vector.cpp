// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#undef NDEBUG

#include <containers/vector.hpp>

#include <cassert>

namespace {

using namespace bounded::literal;

template<std::size_t capacity_, typename T>
void test_generic(T const & t, std::initializer_list<T> init) {
	constexpr auto capacity = bounded::constant<capacity_>;
	using container = containers::vector<T>;
	auto const default_constructed = container{};
	assert(empty(default_constructed));
	assert(default_constructed.capacity() == 0_bi);
	
	assert(begin(default_constructed) == begin(default_constructed));
	assert(begin(default_constructed) == end(default_constructed));
	
	auto const count = container(capacity);
	assert(size(count) == capacity);

	for (auto const & value : count) {
		assert(value == T{});
	}
	
	auto const count_arg = container(capacity, t);
	assert(size(count_arg) == capacity);
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
	using container = containers::vector<int>;
	container v = { 1, 2, 3, 4, 5, 6, 7 };
	erase_if(v, [](auto const & value) { return value % 2 == 0; });
	assert(v == container({ 1, 3, 5, 7 }));
}

auto test_reserve() {
	using container = containers::vector<int>;
	auto v = container{};
	assert(v.capacity() == 0_bi);
	v.reserve(10_bi);
	assert(v.capacity() == 10_bi);
	v.shrink_to_fit();
	assert(v.capacity() == 0_bi);
}

struct complex_resource {
	complex_resource() = default;
	template<typename Size>
	explicit complex_resource(Size size): data(size) {}

	complex_resource(complex_resource const & other) = default;
	complex_resource & operator=(complex_resource const & other) = default;

	containers::vector<int> data;
};

constexpr auto operator==(complex_resource const & lhs, complex_resource const & rhs) noexcept {
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
