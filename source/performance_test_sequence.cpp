// Tests the performance of sequence containers
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

#include <containers/moving_vector/moving_vector.hpp>

#include <boost/timer.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <vector>

namespace {
using value_type = uint8_t;

value_type number() {
	static std::random_device rd;
	static std::mt19937 random_engine(rd());
	static std::uniform_int_distribution<value_type> distribution(0, std::numeric_limits<value_type>::max());
	return distribution(random_engine);
}

class Class {
	public:
		Class() {
			x[0] = number();
		}
		value_type value() const {
			return x[0];
		}
		inline friend bool operator<=(Class const & lhs, Class const & rhs) {
			return lhs.x[0] <= rhs.x[0];
		}
	private:
		// ARRAY_SIZE is defined by the compiler
		std::array<value_type, ARRAY_SIZE> x;
};

template<typename Container>
void add(Container & container, Class const & value) {
	auto const it = std::find_if(std::begin(container), std::end(container), [&](Class const & c) {
		return value <= c;
	});
	container.emplace(it, value);
}

// Do something with the result
template<typename Container>
void insert_to_file(Container const & container) {
	std::fstream file("file.txt");
	for (auto const & c : container) {
		file << c.value() << '\n';
	}
}

template<typename Container>
void f(std::vector<Class> const & values) {
	Container container;
	#if defined RESERVE
	container.reserve(values.size());
	#endif
	for (auto const & value : values) {
		add(container, value);
	}
	insert_to_file(container);
}

}	// namespace

int main(int argc, char ** argv) {
	std::size_t const size = (argc == 1) ? 1 : std::stoul(argv[1]);
	// Default constructor of Class fills in values here
	std::vector<Class> const values_to_be_copied(size);
	#if defined VECTOR
	typedef std::vector<Class> Container;
	#elif defined LIST
	typedef std::list<Class> Container;
	#elif defined DEQUE
	typedef std::deque<Class>Container;
	#elif defined MOVING_VECTOR
	typedef containers::moving_vector<Class> Container;
	#endif
	boost::timer timer;
	f<Container>(values_to_be_copied);
	std::cout << "Finished in " << timer.elapsed() << " seconds.\n";
}
