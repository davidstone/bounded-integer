// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector/static_vector.hpp>
#include <containers/static_vector/make_static_vector.hpp>
#include <containers/array/make_array.hpp>

namespace {

using namespace bounded::literal;

namespace constexpr_static_vector {
	constexpr auto default_constructed = containers::static_vector<int, 9>{};
	static_assert(empty(default_constructed));
	
	constexpr auto default_constructed_elements = containers::static_vector<int, 3>(2_bi);
	static_assert(size(default_constructed_elements) == 2_bi);
	static_assert(at(default_constructed_elements, 1_bi) == 0);
	
	constexpr auto copy_constructed = default_constructed_elements;
	static_assert(copy_constructed == default_constructed_elements);
	
	constexpr auto n_copy = containers::static_vector<int, 4>(4_bi, 10);
	static_assert(size(n_copy) == 4_bi);
	static_assert(at(n_copy, 2_bi) == 10);
	static_assert(n_copy == containers::make_static_vector(4_bi, 10));
	
	constexpr auto make() {
		auto value = containers::static_vector<int, 10>{};
		value.emplace_back(5);
		value.emplace_back(15);
		value.emplace_back(20);
		value.emplace(value.begin() + 1_bi, 10);
		value.pop_back();
		auto copy = containers::static_vector<int, 10>{};
		copy = value;
		copy.insert(copy.begin() + 1_bi, value.begin(), value.end());
		return copy;
	}
	constexpr auto made = make();
	constexpr auto expected = containers::make_array(5, 5, 10, 15, 10, 15);
	static_assert(containers::equal(made.begin(), made.end(), expected.begin(), expected.end()));
	static_assert(made == made);
	
	constexpr auto bounded_integer_element = containers::static_vector<bounded::integer<0, 10>, 1>(1_bi, 2_bi);
	static_assert(front(bounded_integer_element) == 2_bi);
}

template<std::size_t capacity_, typename T>
void test_generic(T const & t, std::initializer_list<T> init) {
	constexpr auto capacity = bounded::constant<capacity_>;
	using container = containers::static_vector<T, capacity_>;
	auto const default_constructed = container{};
	assert(empty(default_constructed));
	static_assert(default_constructed.capacity() == capacity);
	
	auto const count = container(capacity);
	assert(size(count) == capacity);

	assert(default_constructed.begin() == default_constructed.begin());
	assert(default_constructed.begin() == default_constructed.end());
	
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
	assert(std::equal(init_list.begin(), init_list.end(), init.begin(), init.end()));
	
	auto copy = init_list;
	assert(std::equal(copy.begin(), copy.end(), init.begin(), init.end()));
	
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

	assert(data(copy) != data(init_list));
	
	clear(copy);
	push_back(copy, t);
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	copy.pop_back();
	push_back(copy, T(t));
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	clear(copy);
	insert(copy, copy.begin(), t);
	assert(size(copy) == 1_bi);
	assert(back(copy) == t);
	
	assign(copy, init);
	assign(copy, capacity, t);
	
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

struct non_copyable {
	non_copyable() {}
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
	non_copyable & operator=(non_copyable const &) = delete;
	non_copyable & operator=(non_copyable &&) = default;
};

}	// namespace

int main() {
	test_generic<1>(0, { });
	test_generic<1>(0, { 5 });

	test_generic<5>(0, { });
	test_generic<5>(9, { 0, 1, 4 });
	test_generic<5>(-4, { 0, 1, 2, 3, 4 });

	test_generic<3>(std::string("hi"), { std::string(""), std::string("hello"), std::string(100, '=') });
	
	containers::static_vector<int, 10> container = { 1, 2, 3 };
	static_assert(std::is_same<
		containers::index_type<decltype(container)>,
		bounded::checked_integer<0, 9, std::out_of_range>
	>::value);
	static_assert(!containers::is_iterator<containers::static_vector<std::string, 6>>);
	static_assert(containers::is_container<containers::static_vector<std::string, 6>>);

	insert(container, container.begin() + 1_bi, 5_bi, 12);
	auto const expected = { 1, 12, 12, 12, 12, 12, 2, 3 };
	assert(std::equal(container.begin(), container.end(), expected.begin(), expected.end()));
	
	containers::static_vector<non_copyable, 10> test_no_copies;
	test_no_copies.emplace_back();
	insert(test_no_copies, test_no_copies.begin(), non_copyable{});
}
