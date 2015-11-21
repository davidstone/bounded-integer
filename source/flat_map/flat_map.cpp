// A map-like class that has better locality of reference
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

#include <containers/flat_map/flat_map.hpp>
#include <containers/vector/vector.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

namespace {

using namespace bounded::literal;
using namespace containers;
using namespace smart_pointer;

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
	std::cout << "Testing no extra copies or moves.\n" << std::flush;
	stable_flat_map<int, Final> final;
	final.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple());
	final.emplace(std::piecewise_construct, std::forward_as_tuple(6), std::forward_as_tuple(5, 2.0, 'c'));
}


class CheckedMover {
public:
	constexpr CheckedMover(int value) noexcept:
		m_value(value),
		m_moved(false)
	{
	}
		
	constexpr CheckedMover(CheckedMover const & other) noexcept:
		m_value(other.m_value),
		m_moved(other.m_moved)
	{
		assert(!other.m_moved);
		assert(!other.m_destructed);
	}
	constexpr CheckedMover(CheckedMover && other) noexcept:
		m_value(other.m_value),
		m_moved(other.m_moved)
	{
		assert(this != &other);
		assert(!other.m_moved);
		assert(!other.m_destructed);
		other.m_moved = true;
	}
	constexpr CheckedMover & operator=(CheckedMover const & other) noexcept {
		assert(!other.m_moved);
		assert(!other.m_destructed);
		assert(!m_destructed);
		m_value = other.m_value;
		m_moved = other.m_moved;
		return *this;
	}
	constexpr CheckedMover & operator=(CheckedMover && other) noexcept {
		assert(this != &other);
		assert(!other.m_moved);
		assert(!other.m_destructed);
		assert(!m_destructed);
		m_value = other.m_value;
		m_moved = other.m_moved;
		other.m_moved = true;
		return *this;
	}
	~CheckedMover() {
		assert(!m_destructed);
		m_destructed = true;
	}
	
	
	constexpr friend bool operator<(CheckedMover const & lhs, CheckedMover const & rhs) noexcept {
		assert(!lhs.m_moved);
		assert(!rhs.m_moved);
		assert(!lhs.m_destructed);
		assert(!rhs.m_destructed);
		return lhs.m_value < rhs.m_value;
	}
	constexpr friend bool operator==(CheckedMover const & lhs, CheckedMover const & rhs) noexcept {
		assert(!lhs.m_moved);
		assert(!rhs.m_moved);
		assert(!lhs.m_destructed);
		assert(!rhs.m_destructed);
		return lhs.m_value == rhs.m_value;
	}
private:
	int m_value;
	bool m_moved;
	bool m_destructed = false;
};

template<typename Container>
void test_unique_copy_less(Container const & source, Container const & expected) {
	Container destination(size(source), 0);
	auto const it = containers::detail::unique_copy_less(source.begin(), source.end(), destination.begin());
	erase(destination, it, destination.end());
	assert(destination == expected);
}

template<typename Container>
void test_unique_inplace_less(Container source, Container const & expected) {
	auto const it = containers::detail::unique_copy_less(source.begin(), source.end());
	erase(source, it, source.end());
	assert(source == expected);
}

template<typename Container>
void test_unique_less(Container source, Container const & expected) {
	assert(std::is_sorted(source.begin(), source.end()));
	assert(std::is_sorted(expected.begin(), expected.end()));
	test_unique_copy_less(source, expected);
	test_unique_copy_less(std::move(source), expected);
}

template<typename Container>
void test_unique_merge_copy(Container const & lhs, Container const & rhs, Container const & expected) {
	Container result(typename Container::size_type(size(lhs) + size(rhs)), 0);
	auto const it = containers::detail::unique_merge_copy(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), result.begin());
	erase(result, it, result.end());

	assert(result == expected);
}

template<typename Container>
void test_unique_inplace_merge(Container v, Container const & other, Container const & expected) {
	auto const midpoint = static_cast<typename Container::iterator::difference_type>(size(v));
	v.insert(v.end(), other.begin(), other.end());
	auto const it = containers::detail::unique_inplace_merge(v.begin(), v.begin() + midpoint, v.end());
	erase(v, it, v.end());

	assert(v == expected);
}

template<typename Container>
void test_unique_merge(Container v, Container const & other, Container const & expected) {
	assert(std::is_sorted(v.begin(), v.end()));
	assert(std::is_sorted(other.begin(), other.end()));
	test_unique_merge_copy(v, other, expected);
	test_unique_inplace_merge(std::move(v), other, expected);
}

template<typename Container>
void test_unique_specific() {
	test_unique_less<Container>({ 1 }, { 1 });
	test_unique_less<Container>({ 1, 2 }, { 1, 2 });
	test_unique_less<Container>({ }, { });
	test_unique_less<Container>({ 1, 3, 5, 5, 5, 6, 10, 10 }, { 1, 3, 5, 6, 10 });
	test_unique_less<Container>({ 1, 1, 1, 1, 1, 1, 1, 1 }, { 1 });
	test_unique_merge<Container>({ 1, 2, 3, 5 }, { 7, 8, 9 }, { 1, 2, 3, 5, 7, 8, 9 });
	test_unique_merge<Container>({ 1, 3, 5, 7, 9 }, { 2, 2, 2, 3, 3, 6, 7 }, { 1, 2, 3, 5, 6, 7, 9 });
	test_unique_merge<Container>({ 2 }, { 1 }, { 1, 2 });
	test_unique_merge<Container>({ }, { 6 }, { 6 });
	test_unique_merge<Container>({ 4 }, { }, { 4 });
	test_unique_merge<Container>({ }, { }, { });
	test_unique_merge<Container>({ 1 }, { 1 }, { 1 });
	test_unique_merge<Container>({ 8 }, { 8, 8, 8, 8, 8 }, { 8 });
	// Ideally unique_inplace_merge would not assume the first range has no
	// duplicates, but that is my current use-case. I do not know how to remove
	// this limitation without making the algorithm less efficient.
	// test_unique_merge<Container>({ 8, 8 }, { 8, 8, 8, 8, 8 }, { 8 });
}

void test_unique() {
	std::cout << "Testing unique_inplace_merge.\n" << std::flush;
	test_unique_specific<std::vector<CheckedMover>>();
	test_unique_specific<vector<CheckedMover>>();
	test_unique_specific<containers::moving_vector<CheckedMover>>();
}

template<typename... Ts>
constexpr auto size(std::map<Ts...> const & std_map) {
	return std_map.size();
}

template<typename container_type>
void test() {
	std::cout << "Testing many member functions.\n" << std::flush;

	container_type empty;
	std::initializer_list<typename container_type::value_type> const init = { {1, 2}, {2, 5}, {3, 3} };
	container_type container(init);
	assert((container == container_type{init}));
	container.emplace(std::make_pair(4, 4));
	container.emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple(3));
	assert(container.at(5) == 3);
	assert(size(container) == 5_bi);
	container.emplace(typename container_type::value_type(3, 10));
	assert(size(container) == 5_bi);
	assert(container.at(3) == 3);
}

#define TRACK_COMPARISONS
#if defined TRACK_COMPARISONS
std::size_t number_of_comparisons = 0;
template<typename Key>
class TrackerCompare {
public:
	bool operator()(Key const & lhs, Key const & rhs) const {
		++number_of_comparisons;
		return lhs < rhs;
	}
};

template<typename Key>
using Compare = TrackerCompare<Key>;

#else
template<typename Key>
using Compare = std::less<Key>;
#endif

#if defined USE_SYSTEM_MAP
template<typename Key, typename Value>
using map_type = std::map<Key, Value, Compare<Key>>;
#elif defined USE_UNSTABLE_FLAT_MAP
template<typename Key, typename Value>
using map_type = unstable_flat_map<Key, Value, Compare<Key>>;
#else
template<typename Key, typename Value>
using map_type = stable_flat_map<Key, Value, Compare<Key>>;
#endif

using unit = std::chrono::milliseconds;
class TimeDestructor {
public:
	using TimePoint = decltype(std::chrono::high_resolution_clock::now());
	void set() {
		m_time_point = std::chrono::high_resolution_clock::now();
	}
	~TimeDestructor() {
		auto const now = std::chrono::high_resolution_clock::now();
		std::cout << "Destroyed: " << std::chrono::duration_cast<unit>(now - m_time_point).count() << '\n';
	}
private:
	TimePoint m_time_point = TimePoint{};
};

template<typename Key, typename Value>
void test_performance(std::size_t const loop_count) {
	auto const generator = [](std::size_t size) {
		static boost::random::mt19937 engine(0);
		static boost::random::uniform_int_distribution<std::uint32_t> distribution;
		std::vector<std::pair<Key, Value>> source;
		source.reserve(size);
		for (std::size_t n = 0; n != size; ++n) {
			source.emplace_back(distribution(engine), distribution(engine));
		}
		return source;
	};

	auto const source = generator(loop_count);
	auto const additional_batch = generator(loop_count);
	auto const additional = generator(static_cast<std::size_t>(std::log2(loop_count)));
	
	std::cout << '\n';
	using std::chrono::high_resolution_clock;
	auto const start = high_resolution_clock::now();

	TimeDestructor destructor;
	map_type<Key, Value> map(source.begin(), source.end());
	auto const constructed = high_resolution_clock::now();
	#if defined TRACK_COMPARISONS
		auto const constructed_comparisons = number_of_comparisons;
	#endif

	for (auto const & value : source) {
		auto const volatile it = map.find(value.first);
		static_cast<void>(it);
	}
	auto const found = high_resolution_clock::now();
	#if defined TRACK_COMPARISONS
		auto const found_comparisons = number_of_comparisons;
	#endif

	map.insert(additional_batch.begin(), additional_batch.end());
	auto const inserted_batch = high_resolution_clock::now();
	#if defined TRACK_COMPARISONS
		auto const inserted_batch_comparisons = number_of_comparisons;
	#endif

	for (auto const & value : additional) {
		map.insert(value);
	}
	auto const inserted = high_resolution_clock::now();
	#if defined TRACK_COMPARISONS
		auto const inserted_comparisons = number_of_comparisons;
	#endif

	auto map2 = map;
	auto const copied = high_resolution_clock::now();
	
	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	auto const iterated = high_resolution_clock::now();

	for (auto const & value : source) {
		auto const volatile it = map.find(value.first);
		static_cast<void>(it);
	}
	auto const found_in_extras = high_resolution_clock::now();
	#if defined TRACK_COMPARISONS
		auto const found_with_extra_comparisons = number_of_comparisons;
	#endif
	std::cout << "map size: " << size(map) << "\n\n";
	
	#if defined TRACK_COMPARISONS
		std::cout << "Constructed comparisons: " << constructed_comparisons << '\n';
		std::cout << "Found comparisons: " << found_comparisons - constructed_comparisons << '\n';
		std::cout << "Batch insertion comparisons: " << inserted_batch_comparisons - found_comparisons << '\n';
		std::cout << "Insertion comparisons: " << inserted_comparisons - inserted_batch_comparisons << '\n';
		std::cout << "Found with extras comparisons: " << found_with_extra_comparisons - inserted_comparisons << '\n';
		std::cout << "Number of comparisons: " << number_of_comparisons << '\n';
		std::cout << '\n';
	#endif


	std::cout << "Construction time: " << std::chrono::duration_cast<unit>(constructed - start).count() << '\n';
	std::cout << "Found time: " << std::chrono::duration_cast<unit>(found - constructed).count() << '\n';
	std::cout << "Batch insertion time: " << std::chrono::duration_cast<unit>(inserted_batch - found).count() << '\n';
	std::cout << "Insertion time: " << std::chrono::duration_cast<unit>(inserted - inserted_batch).count() << '\n';
	std::cout << "Copying time: " << std::chrono::duration_cast<unit>(copied - inserted).count() << '\n';
	std::cout << "Iteration time: " << std::chrono::duration_cast<unit>(iterated - copied).count() << '\n';
	std::cout << "Found time with extra elements: " << std::chrono::duration_cast<unit>(found_in_extras - iterated).count() << '\n';
	destructor.set();
}

template<std::size_t size>
class Class {
public:
	Class(std::uint32_t v) {
		m_value[0] = v;
	}
	std::uint32_t value() const {
		return m_value[0];
	}
private:
	std::array<std::uint32_t, size> m_value;
};

template<std::size_t size>
bool operator<(Class<size> const & lhs, Class<size> const & rhs) {
	return lhs.value() < rhs.value();
}

}	// namespace

int main(int argc, char ** argv) {
	std::cout.sync_with_stdio(false);
	test_unique();
	test_no_extra_copy_or_move();
	test<map_type<int, int>>();

	auto const loop_count = (argc == 1) ? 1 : std::stoull(argv[1]);
	std::cout << "Testing performance.\n" << std::flush;
	test_performance<Class<1>, Class<1>>(loop_count);
}
