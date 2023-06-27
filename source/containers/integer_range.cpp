// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/bracket.hpp>

export module containers.integer_range;

import containers.algorithms.compare;
import containers.algorithms.transform;
import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.iter_value_t;

import bounded;
import numeric_traits;
import std_module;

using namespace ::bounded::literal;

namespace containers {

template<bool use_bounded, auto value_min, auto sentinel_max, typename Step>
struct integer_range_iterator {
private:
	using storage_type = bounded::integer<value_min, sentinel_max>;
	using bounded_type = bounded::integer<
		bounded::builtin_min_value<storage_type>,
		bounded::normalize<bounded::max(numeric_traits::min_value<storage_type>, numeric_traits::max_value<storage_type> - numeric_traits::min_value<Step>)>
	>;

public:
	using difference_type = decltype((bounded::declval<storage_type>() - bounded::declval<storage_type>()) / bounded::declval<Step>());

	integer_range_iterator() = default;
	explicit constexpr integer_range_iterator(storage_type const value, Step const step):
		m_value(value),
		m_step(step)
	{
	}
	
	friend constexpr auto operator-(integer_range_iterator const lhs, integer_range_iterator const rhs) {
		BOUNDED_ASSERT(lhs.m_step == rhs.m_step);
		return (lhs.m_value - rhs.m_value) / lhs.m_step;
	}

	friend constexpr auto operator+(integer_range_iterator const lhs, difference_type const rhs) {
		if constexpr (has_no_values) {
			std::unreachable();
			return lhs;
		} else {
			return integer_range_iterator(bounded::assume_in_range<storage_type>(lhs.m_value + rhs * lhs.m_step), lhs.m_step);
		}
	}
	
	friend constexpr auto operator+(difference_type const lhs, integer_range_iterator const rhs) {
		return rhs + lhs;
	}

	// Not technically needed, but helps compile faster
	friend constexpr auto & operator++(integer_range_iterator & it) {
		if constexpr (has_no_values) {
			std::unreachable();
		} else {
			it.m_value += it.m_step;
		}
		return it;
	}

	friend constexpr auto operator<=>(integer_range_iterator, integer_range_iterator) = default;

	// It is undefined behavior to dereference a past-the-end iterator.
	constexpr auto operator*() const {
		using value_type = std::conditional_t<
			use_bounded,
			bounded_type,
			typename bounded_type::underlying_type
		>;
		return ::bounded::assume_in_range<value_type>(m_value);
	}

private:
	static constexpr auto has_no_values = numeric_traits::max_value<difference_type> == 0_bi;

	[[no_unique_address]] storage_type m_value;
	[[no_unique_address]] Step m_step;
};

export template<bounded::integral Integer, bounded::integral Sentinel = Integer, bounded::integral Step = bounded::constant_t<1>>
struct integer_range {
	static_assert(numeric_traits::max_value<Sentinel> >= numeric_traits::min_value<Integer>, "Cannot construct inverted integer ranges.");

	using iterator = integer_range_iterator<
		bounded::bounded_integer<Integer>,
		bounded::builtin_min_value<Integer>,
		bounded::builtin_max_value<Sentinel>,
		Step
	>;
	
	// If `last` is not reachable by adding `step` to `first` some number of
	// times, the behavior is undefined.
	constexpr integer_range(Integer const first, Sentinel const last, Step const step):
		m_begin(first),
		m_end(last),
		m_step(step)
	{
		BOUNDED_ASSERT((last - first) % step == 0_bi);
	}

	constexpr integer_range(Integer const first, Sentinel const last):
		integer_range(first, last, Step(1_bi))
	{
	}
	constexpr integer_range(Sentinel const size):
		integer_range(Integer(0_bi), size)
	{
	}

	constexpr auto begin() const {
		return iterator(m_begin, m_step);
	}
	constexpr auto end() const {
		return iterator(m_end, m_step);
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

private:
	[[no_unique_address]] Integer m_begin;
	[[no_unique_address]] Sentinel m_end;
	[[no_unique_address]] Step m_step;
};

template<typename Size>
integer_range(Size) -> integer_range<
	std::conditional_t<bounded::bounded_integer<Size>, bounded::constant_t<0>, Size>,
	Size
>;

export template<typename Step = bounded::constant_t<1>>
constexpr auto inclusive_integer_range(auto const first, auto const last, Step const step = 1_bi) {
	return integer_range(first, last + step, step);
}

export constexpr auto inclusive_integer_range(auto const last) {
	return inclusive_integer_range(0_bi, last);
}

// Technically this could construct a more efficient object for compile-time
// constant arguments, which will be almost all of them. However, it is hard to
// make this a nice interface until we get constexpr function parameters.
export template<typename Enum> requires std::is_enum_v<Enum>
constexpr auto enum_range(Enum const first, Enum const last) {
	return containers::transform(
		inclusive_integer_range(bounded::integer(first), bounded::integer(last)),
		bounded::construct<Enum>
	);
}

export template<typename Enum> requires (
	std::is_enum_v<Enum> and
	bounded::integer(numeric_traits::min_value<Enum>) == 0_bi
)
constexpr auto enum_range(Enum const last) {
	return enum_range(static_cast<Enum>(0), last);
}

export template<typename Enum> requires std::is_enum_v<Enum>
constexpr auto enum_range() {
	return enum_range(numeric_traits::min_value<Enum>, numeric_traits::max_value<Enum>);
}

} // namespace containers

static_assert(containers::equal(
	containers::integer_range(0_bi),
	containers::array<bounded::integer<0, 0>, 0_bi>()
));
static_assert(containers::equal(
	containers::integer_range(3_bi),
	containers::array{0_bi, 1_bi, 2_bi}
));
static_assert(containers::equal(
	containers::integer_range(2_bi, 4_bi),
	containers::array{2_bi, 3_bi}
));
static_assert(containers::equal(
	containers::integer_range(2_bi, 6_bi, 2_bi),
	containers::array{2_bi, 4_bi}
));

static_assert(containers::integer_range(3_bi)[1_bi] == 1_bi);

static_assert(containers::equal(
	containers::integer_range(bounded::integer<1, 2>(1_bi), bounded::integer<3, 5>(3_bi)),
	containers::array{1_bi, 2_bi}
));

static_assert([]{
	for (auto const n [[maybe_unused]] : containers::integer_range(1_bi)) {
	}
	return true;
}());
