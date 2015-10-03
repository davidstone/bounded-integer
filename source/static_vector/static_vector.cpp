// std::vector-like interface around an array
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <containers/static_vector/static_vector.hpp>

#include <iostream>

namespace {

using namespace bounded::literal;

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

	assert(copy.data() != init_list.data());
	
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

}	// namespace

int main() {
	test_generic<1>(0, { });
	test_generic<1>(0, { 5 });

	test_generic<5>(0, { });
	test_generic<5>(9, { 0, 1, 4 });
	test_generic<5>(-4, { 0, 1, 2, 3, 4 });

	test_generic<3>(std::string("hi"), { std::string(""), std::string("hello"), std::string(100, '=') });
}
