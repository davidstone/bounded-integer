// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.concatenate_view;

import containers.algorithms.compare;
import containers.algorithms.concatenate_view;

import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.forward_random_access_range;
import containers.iterator;
import containers.range;
import containers.range_value_t;
import containers.sentinel_for;
import containers.size;
import containers.subrange;

import bounded;
import std_module;

using namespace bounded::literal;

template<typename LHS, typename RHS>
constexpr bool equal_values_and_types(LHS const & lhs, RHS const & rhs) {
	static_assert(std::same_as<containers::range_value_t<LHS>, containers::range_value_t<RHS>>);
	return containers::equal(lhs, rhs);
}

constexpr auto array1 = containers::array{1, 2, 3};
constexpr auto array2 = containers::array{2, 3, 5, 7};
constexpr auto array3 = containers::array{1, 1, 2, 3, 5};

constexpr auto two = containers::concatenate_view(array1, array2);

using iterator = decltype(begin(two));
using sentinel = decltype(end(two));

static_assert(containers::iterator<iterator>);
static_assert(containers::sentinel_for<sentinel, iterator>);
static_assert(containers::range<decltype(two)>);

static_assert(containers::size(two) == containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	two,
	containers::array{1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(two) + containers::size(two) == end(two));
static_assert(begin(two) + containers::size(two) - begin(two) == containers::size(two));

constexpr auto three = containers::concatenate_view(array3, array1, array2);
static_assert(containers::size(three) == containers::size(array3) + containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	three,
	containers::array{1, 1, 2, 3, 5, 1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(three) + containers::size(three) == end(three));
static_assert(begin(three) + containers::size(three) - begin(three) == containers::size(three));

static_assert(*(begin(three) + 7_bi) == 3);

constexpr auto from_temp = containers::concatenate_view(containers::array{1}, containers::array{2});
static_assert(equal_values_and_types(from_temp, containers::array{1, 2}));

static_assert(containers::forward_random_access_range<containers::concatenate_view<std::string_view>>);

using containers::operator+;
using containers::operator-;
using containers::operator+=;
using containers::operator-=;
using containers::operator++;
using containers::operator++;

struct input_iterator {
	using difference_type = bounded::integer<-2, 2>;
	input_iterator() = default;
	input_iterator(input_iterator &&) = default;
	input_iterator(input_iterator const &) = delete;
	auto operator=(input_iterator &&) -> input_iterator & = default;
	auto operator=(input_iterator const &) -> input_iterator & = delete;

	constexpr auto operator*() const -> int {
		return m_value;
	}
	friend constexpr auto operator+(input_iterator lhs, bounded::constant_t<1>) -> input_iterator {
		++lhs.m_value;
		return lhs;
	}
	friend constexpr auto operator==(input_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_value == 2;
	}

private:
	int m_value = 0;
};

static_assert(containers::equal(
	containers::concatenate_view(
		containers::subrange<input_iterator, std::default_sentinel_t>(input_iterator(), std::default_sentinel),
		containers::subrange(input_iterator(), std::default_sentinel)
	),
	containers::array({0, 1, 0, 1})
));