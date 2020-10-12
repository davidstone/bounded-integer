// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/flat_map.hpp>
#include <containers/algorithms/filter_iterator.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/emplace_back.hpp>
#include <containers/vector.hpp>

#include <operators/forward.hpp>

#include "../test_assert.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <vector>

namespace {

using namespace bounded::literal;

using base = containers::array<containers::map_value_type<int, int>, 3>;
constexpr auto constexpr_constructible = containers::basic_flat_map<base>(
	containers::assume_sorted_unique,
	base{{
		{0, 4},
		{1, 2},
		{3, 5}
	}}
);
static_assert(constexpr_constructible.at(0) == 4);
static_assert(constexpr_constructible.at(1) == 2);
static_assert(constexpr_constructible.at(3) == 5);
static_assert(constexpr_constructible.find(2) == end(constexpr_constructible));

template<typename container_type>
void test() {
	std::cout << "Testing many member functions.\n" << std::flush;

	auto empty = container_type();
	auto const init = std::initializer_list<typename container_type::value_type>{{1, 2}, {2, 5}, {3, 3}};
	auto container = container_type(init);
	BOUNDED_TEST((container == container_type(init)));
	container.insert(typename container_type::value_type(4, 4));
	container.try_emplace(5, 3);
	BOUNDED_TEST(container.at(5) == 3);
	BOUNDED_TEST(size(container) == 5_bi);
	container.insert(typename container_type::value_type(3, 10));
	BOUNDED_TEST(size(container) == 5_bi);
	BOUNDED_TEST(container.at(3) == 3);
}

#define TRACK_EXTRACTIONS
#if defined TRACK_EXTRACTIONS
std::size_t number_of_extractions = 0;
template<typename Extract>
struct extract_tracker {
	decltype(auto) operator()(auto const & key) const {
		++number_of_extractions;
		return Extract()(key);
	}
};

template<typename Extract>
using extract_key_t = extract_tracker<Extract>;

#else
template<typename Extract>
using extract_key_t = Extract;
#endif


#if defined USE_SYSTEM_MAP
	template<typename Key, typename Value, typename Extract>
	using map_type = std::map<
		Key,
		Value,
		containers::detail::extract_key_to_compare<extract_key_t<Extract>>
	>;

	template<typename Key, typename Value>
	using value_type = std::pair<Key, Value>;

	constexpr auto const & get_key(auto const & pair) {
		return pair.first;
	}

	template<typename Map>
	auto construct_from_range(auto && range) {
		return Map(OPERATORS_FORWARD(begin(range)), OPERATORS_FORWARD(end(range)));
	}

	void insert_range(auto & map, auto && range) {
		map.insert(OPERATORS_FORWARD(begin(range)), OPERATORS_FORWARD(end(range)));
	}

#elif defined USE_FLAT_MAP
	template<typename Key, typename Value, typename Extract>
	using map_type = containers::flat_map<Key, Value, extract_key_t<Extract>>;

	template<typename Key, typename Value>
	using value_type = containers::map_value_type<Key, Value>;

	constexpr auto const & get_key(auto const & pair) {
		return pair.key();
	}

	template<typename Map>
	auto construct_from_range(auto && range) {
		return Map(OPERATORS_FORWARD(range));
	}

	void insert_range(auto & map, auto && range) {
		map.insert(OPERATORS_FORWARD(range));
	}

#else
	#error
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

template<std::size_t size>
struct Thing {
public:
	Thing(std::uint32_t v) {
		m_value[0] = v;
	}

	friend struct Extract;
private:
	std::array<std::uint32_t, size> m_value;
};

struct Extract {
	template<std::size_t size>
	auto operator()(Thing<size> const thing) const -> std::uint32_t {
		return thing.m_value[0];
	}
};

template<std::size_t key_size, std::size_t value_size>
void test_performance(std::size_t const loop_count) {
	auto const generator = [](std::size_t size) {
		static std::mt19937 engine(0);
		static std::uniform_int_distribution<std::uint32_t> distribution;
		std::vector<value_type<Thing<key_size>, Thing<value_size>>> source;
		source.reserve(size);
		for (std::size_t n = 0; n != size; ++n) {
			::containers::emplace_back(source, distribution(engine), distribution(engine));
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
	auto map = construct_from_range<map_type<Thing<key_size>, Thing<value_size>, Extract>>(source);
	auto const constructed = high_resolution_clock::now();
	#if defined TRACK_EXTRACTIONS
		auto const constructed_extractions = number_of_extractions;
	#endif

	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	for (auto const & value : source) {
		auto const volatile it [[maybe_unused]] = map.find(get_key(value));
	}
	auto const found = high_resolution_clock::now();
	#if defined TRACK_EXTRACTIONS
		auto const found_extractions = number_of_extractions;
	#endif

	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	insert_range(map, additional_batch);
	auto const inserted_batch = high_resolution_clock::now();
	#if defined TRACK_EXTRACTIONS
		auto const inserted_batch_extractions = number_of_extractions;
	#endif

	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	for (auto const & value : additional) {
		map.insert(value);
	}
	auto const inserted = high_resolution_clock::now();
	#if defined TRACK_EXTRACTIONS
		auto const inserted_extractions = number_of_extractions;
	#endif

	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	auto map2 = map;
	auto const copied = high_resolution_clock::now();
	
	for (auto const & value : map) {
		auto const volatile & thing = value;
		auto ignore = [](auto &&){};
		ignore(thing);
	}
	auto const iterated = high_resolution_clock::now();

	for (auto const & value : source) {
		auto const volatile it [[maybe_unused]] = map.find(get_key(value));
	}
	auto const found_in_extras = high_resolution_clock::now();
	#if defined TRACK_EXTRACTIONS
		auto const found_with_extra_extractions = number_of_extractions;
	#endif
	std::cout << "map size: " << size(map) << "\n\n";
	
	#if defined TRACK_EXTRACTIONS
		std::cout << "Constructed extractions: " << constructed_extractions << '\n';
		std::cout << "Found extractions: " << found_extractions - constructed_extractions << '\n';
		std::cout << "Batch insertion extractions: " << inserted_batch_extractions - found_extractions << '\n';
		std::cout << "Insertion extractions: " << inserted_extractions - inserted_batch_extractions << '\n';
		std::cout << "Found with extras extractions: " << found_with_extra_extractions - inserted_extractions << '\n';
		std::cout << "Number of extractions: " << number_of_extractions << '\n';
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

}	// namespace

int main(int argc, char ** argv) {
	auto const loop_count = (argc == 1) ? 1 : std::stoull(argv[1]);
	if (loop_count == 0) {
		std::cerr << "Must pass a positive number\n";
		return 1;
	}

	test<map_type<int, int, bounded::identity_t>>();

	std::cout << "Testing performance.\n" << std::flush;
	test_performance<1, 1>(loop_count);
}
