// std::vector-like interface around a bounded::array
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
	assert(default_constructed.empty());
	static_assert(default_constructed.capacity() == capacity);
	
	auto const count = container(capacity);
	assert(count.size() == capacity);
	
	for (auto const & value : count) {
		assert(value == T{});
	}
	
	auto const count_arg = container(capacity, t);
	assert(count.size() == capacity);
	for (auto const & value : count_arg) {
		assert(value == t);
	}
	assert(count_arg.front() == t);
	assert(count_arg.back() == t);
	assert(count_arg[0_bi] == t);
	assert(count_arg.at(0_bi) == t);
	try {
		count_arg.at(static_cast<unsigned>(capacity + 1_bi));
		assert(false);
	} catch (std::out_of_range const &) {
	}

	auto const init_list = container(init);
	assert(std::equal(init_list.begin(), init_list.end(), init.begin(), init.end()));
	
	auto copy = init_list;
	assert(std::equal(copy.begin(), copy.end(), init.begin(), init.end()));
	
	auto move = std::move(copy);
	copy.clear();
	assert(move == init_list);
	
	copy = move;
	assert(copy.size() == init.size());
	assert(copy == move);
	
	move = std::move(copy);
	copy.clear();
	assert(copy.empty());
	
	assert(copy == default_constructed);
	
	copy = init;
	assert(copy == init_list);

	assert(copy.data() != init_list.data());
	
	copy.clear();
	copy.push_back(t);
	assert(copy.size() == 1_bi);
	assert(copy.back() == t);
	copy.pop_back();
	copy.push_back(T(t));
	assert(copy.size() == 1_bi);
	assert(copy.back() == t);
	copy.clear();
	copy.insert(copy.begin(), t);
	assert(copy.size() == 1_bi);
	assert(copy.back() == t);
	
	copy.assign(init);
	copy.assign(capacity, t);
	
	// TODO: insert(it, it, it) overload
	auto const old_front = copy.front();
	copy.resize(capacity);
	assert(copy.front() == old_front);
	copy.clear();
	copy.resize(capacity);
	assert(copy.front() == T{});
	assert(copy.size() == capacity);
	copy.resize(0_bi);
	assert(copy.empty());
	copy.resize(capacity, t);
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
