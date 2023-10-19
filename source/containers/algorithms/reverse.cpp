// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.reverse;

import containers.algorithms.copy;

import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.common_iterator_functions;
import containers.range;

import bounded;
import std_module;

namespace containers {

export template<bidirectional_range Range>
constexpr auto reverse(Range && r) {
	auto first = containers::begin(r);
	auto last = containers::end(r);
	if constexpr (random_access_range<Range>) {
		if (first == last) {
			return;
		}
		--last;
		while (first < last) {
			std::ranges::swap(*first, *last);
			++first;
			--last;
		}
	} else {
		while (first != last) {
			--last;
			if (first == last) {
				break;
			}
			std::ranges::swap(*first, *last);
			++first;
		}
	}
}

} // namespace containers

constexpr auto random_access_reversed = [] {
	auto range = containers::array({0, 1, 2, 3, 4});
	containers::reverse(range);
	return range;
}();
static_assert(random_access_reversed == containers::array({4, 3, 2, 1, 0}));


static constexpr auto bidirectional_size = 5;

struct bidirectional_iterator {
	using difference_type = bounded::integer<-bidirectional_size, bidirectional_size>;

	constexpr explicit bidirectional_iterator(int * ptr):
		m_ptr(ptr)
	{
	}

	constexpr auto & operator*() const {
		return *m_ptr;
	}

	friend auto operator==(bidirectional_iterator, bidirectional_iterator) -> bool = default;

	friend constexpr auto operator++(bidirectional_iterator & lhs) -> bidirectional_iterator & {
		++lhs.m_ptr;
		return lhs;
	}
	friend constexpr auto operator--(bidirectional_iterator & lhs) -> bidirectional_iterator & {
		--lhs.m_ptr;
		return lhs;
	}
private:
	int * m_ptr = nullptr;
};

struct bidirectional_container {
	constexpr explicit bidirectional_container(containers::c_array<int, bidirectional_size> && value) {
		containers::copy(value, +m_value);
	}
	constexpr auto begin() {
		return bidirectional_iterator(m_value);
	}
	constexpr auto end() {
		return bidirectional_iterator(m_value + bidirectional_size);
	}

	friend auto operator==(bidirectional_container const &, bidirectional_container const &) -> bool = default;

private:
	int m_value[bidirectional_size];
};

constexpr auto bidirectional_reversed = [] {
	auto range = bidirectional_container({0, 1, 2, 3, 4});
	containers::reverse(range);
	return range;
}();
static_assert(bidirectional_reversed == bidirectional_container({4, 3, 2, 1, 0}));
