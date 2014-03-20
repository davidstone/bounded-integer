// A map-like class that has better locality of reference
// Copyright (C) 2014 David Stone
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

#include "flat_map.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

using namespace containers;
using namespace smart_pointer;

namespace {

class Final {
public:
	constexpr Final() noexcept {}
	constexpr Final(int, double, char) noexcept {}
	Final(Final const & other) = delete;
	Final(Final && other) = delete;
	Final & operator=(Final const & other) = delete;
	Final & operator=(Final && other) = delete;
};

void test_no_extra_copy_or_move() {
	stable_flat_map<int, Final> final;
	final.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple());
	final.emplace(std::piecewise_construct, std::forward_as_tuple(6), std::forward_as_tuple(5, 2.0, 'c'));
}

template<typename container_type>
void test() {
	container_type empty;
	std::initializer_list<typename container_type::value_type> const init = { {1, 2}, {2, 5}, {3, 3} };
	container_type container(init);
	assert((container == container_type{init}));
	container.emplace(std::make_pair(4, 4));
	container.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple(3));
	assert(container.at(5) == 3);
	assert(container.size() == 5);
	container.emplace(typename container_type::value_type(3, 10));
	assert(container.size() == 5);
	assert(container.at(3) == 3);
}


#if defined USE_SYSTEM_MAP
template<typename Key, typename Value>
using map_type = std::map<Key, Value>;
#elif defined USE_UNSTABLE_FLAT_MAP
template<typename Key, typename Value>
using map_type = unstable_flat_map<Key, Value>;
#else
template<typename Key, typename Value>
using map_type = stable_flat_map<Key, Value>;
#endif

template<typename T>
T generate_random_value(std::mt19937 & engine) {
	static std::uniform_int_distribution<T> distribution;
	return distribution(engine);
}

template<typename Key, typename Value>
using value_type = std::pair<Key const, Value>;

template<typename Key, typename Value>
std::vector<value_type<Key, Value>> generate_random_values(std::size_t size, std::mt19937 & engine) {
	std::vector<value_type<Key, Value>> source;
	for (std::size_t n = 0; n != size; ++n) {
		source.emplace_back(generate_random_value<Key>(engine), generate_random_value<Value>(engine));
	}
	return source;
}

template<typename Key, typename Value>
void test_performance(std::size_t const loop_count) {
	std::random_device rd;
	std::mt19937 engine(rd());

	auto const generator = [&](std::size_t size) { return generate_random_values<Key, Value>(size, engine); };
	auto const source = generator(loop_count);
	auto const additional_batch = generator(loop_count);
	auto const additional = generator(loop_count / 100);
	using std::chrono::high_resolution_clock;
	auto const start = high_resolution_clock::now();
	map_type<Key, Value> map(source.begin(), source.end());
	auto const constructed = high_resolution_clock::now();

	for (auto const & value : source) {
		auto const volatile it = map.find(value.first);
		static_cast<void>(it);
	}
	auto const found = high_resolution_clock::now();

	map.insert(additional_batch.begin(), additional_batch.end());
	auto const inserted_batch = high_resolution_clock::now();

	#if 0
	for (auto const & value : additional) {
		map.insert(value);
	}
	#endif
	auto const inserted = high_resolution_clock::now();

	for (auto const & value : additional) {
		map.insert(value);
	}
	auto const not_inserted = high_resolution_clock::now();

	auto map2 = map;
	auto const copied = high_resolution_clock::now();
	
	for (auto const & value : map) {
		auto const volatile & thing = value;
	}
	auto const iterated = high_resolution_clock::now();
	for (auto const & value : source) {
		auto const volatile it = map.find(value.first);
		static_cast<void>(it);
	}
	auto const found_in_extras = high_resolution_clock::now();

	typedef std::chrono::milliseconds unit;
	std::cout << "Construction time: " << std::chrono::duration_cast<unit>(constructed - start).count() << '\n';
	std::cout << "Found time: " << std::chrono::duration_cast<unit>(found - constructed).count() << '\n';
	std::cout << "Batch insertion time: " << std::chrono::duration_cast<unit>(inserted_batch - found).count() << '\n';
	std::cout << "Insertion time: " << std::chrono::duration_cast<unit>(inserted - inserted_batch).count() << '\n';
	std::cout << "Non-insertion time: " << std::chrono::duration_cast<unit>(not_inserted - inserted).count() << '\n';
	std::cout << "Copying time: " << std::chrono::duration_cast<unit>(copied - not_inserted).count() << '\n';
	std::cout << "Iteration time: " << std::chrono::duration_cast<unit>(iterated - copied).count() << '\n';
	std::cout << "Found time with extra elements: " << std::chrono::duration_cast<unit>(found_in_extras - iterated).count() << '\n';
}

}	// namespace

int main(int argc, char ** argv) {
	std::cout << "Testing no extra copies or moves.\n" << std::flush;
	test_no_extra_copy_or_move();
	std::cout << "Testing many member functions.\n" << std::flush;
	// gcc's std::map doesn't have emplace
	// test<map_type<int, int>>();
	test<stable_flat_map<int, int>>();
	test<unstable_flat_map<int, int>>();

	auto const loop_count = (argc == 1) ? 1 : std::stoull(argv[1]);
	std::cout << "Testing performance.\n" << std::flush;
	test_performance<std::uint32_t, std::uint32_t>(loop_count);
}
