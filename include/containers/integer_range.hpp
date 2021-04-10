// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform.hpp>
#include <containers/begin_end.hpp>
#include <containers/common_functions.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <operators/bracket.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<bool use_bounded, auto value_min, auto sentinel_max, typename Step>
struct integer_range_iterator {
private:
	using storage_type = bounded::integer<value_min, sentinel_max>;
	using bounded_type = bounded::integer<
		bounded::detail::builtin_min_value<storage_type>,
		bounded::normalize<bounded::max(bounded::min_value<storage_type>, bounded::max_value<storage_type> - bounded::min_value<Step>).value()>
	>;

public:
	using value_type = std::conditional_t<
		use_bounded,
		bounded_type,
		typename bounded_type::underlying_type
	>;
	using difference_type = decltype((std::declval<storage_type>() - std::declval<storage_type>()) / std::declval<Step>());
	using pointer = value_type const *;
	using reference = value_type;
	using iterator_category = std::random_access_iterator_tag;

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

	friend constexpr auto operator+(integer_range_iterator const lhs, bounded::convertible_to<difference_type> auto const rhs) {
		if constexpr (has_no_values) {
			bounded::unreachable();
			return lhs;
		} else {
			return integer_range_iterator(storage_type(lhs.m_value + rhs * lhs.m_step), lhs.m_step);
		}
	}
	
	friend constexpr auto operator+(difference_type const lhs, integer_range_iterator const rhs) {
		return rhs + lhs;
	}

	// Not technically needed, but helps compile faster
	friend constexpr auto & operator++(integer_range_iterator & it) {
		if constexpr (has_no_values) {
			bounded::unreachable();
		} else {
			it.m_value += it.m_step;
		}
		return it;
	}

	friend constexpr auto operator<=>(integer_range_iterator, integer_range_iterator) = default;

	// It is undefined behavior to dereference a past-the-end iterator.
	constexpr auto operator*() const {
		return value_type(m_value);
	}

private:
	static constexpr auto has_no_values = bounded::max_value<difference_type> == bounded::constant<0>;

	[[no_unique_address]] storage_type m_value;
	[[no_unique_address]] Step m_step;
};

}	// namespace detail

template<bounded::integral Integer, bounded::integral Sentinel = Integer, bounded::integral Step = bounded::constant_t<1>>
struct integer_range {
	static_assert(bounded::max_value<Sentinel> >= bounded::min_value<Integer>, "Cannot construct inverted integer ranges.");

	using iterator = detail::integer_range_iterator<
		bounded::bounded_integer<Integer>,
		bounded::detail::builtin_min_value<Integer>,
		bounded::detail::builtin_max_value<Sentinel>,
		Step
	>;
	using const_iterator = iterator;

	using value_type = iter_value_t<iterator>;
	using size_type = bounded::integer<0, bounded::detail::builtin_max_value<iter_difference_t<iterator>>>;
	
	// If `last` is not reachable by adding `step` to `first` some number of
	// times, the behavior is undefined.
	constexpr integer_range(Integer const first, Sentinel const last, Step const step):
		m_begin(first),
		m_end(last),
		m_step(step)
	{
	}

	constexpr integer_range(Integer const first, Sentinel const last):
		integer_range(first, last, Step(bounded::constant<1>))
	{
	}
	constexpr integer_range(Sentinel const size):
		integer_range(Integer(bounded::constant<0>), size)
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

template<typename Step = bounded::constant_t<1>>
constexpr auto inclusive_integer_range(auto const first, auto const last, Step const step = bounded::constant<1>) {
	return integer_range(first, last + step, step);
}

constexpr auto inclusive_integer_range(auto const last) {
	return inclusive_integer_range(bounded::constant<0>, last);
}

// Technically this could construct a more efficient object for compile-time
// constant arguments, which will be almost all of them. However, it is hard to
// make this a nice interface until we get constexpr function parameters.
template<typename Enum> requires std::is_enum_v<Enum>
constexpr auto enum_range(Enum const first, Enum const last) {
	return containers::transform(
		inclusive_integer_range(bounded::integer(first), bounded::integer(last)),
		[](auto e) { return static_cast<Enum>(e); }
	);
}

template<typename Enum> requires (
	std::is_enum_v<Enum> and
	bounded::integer(bounded::min_value<Enum>) == bounded::constant<0>
)
constexpr auto enum_range(Enum const last) {
	return enum_range(static_cast<Enum>(0), last);
}

template<typename Enum> requires std::is_enum_v<Enum>
constexpr auto enum_range() {
	return enum_range(bounded::min_value<Enum>, bounded::max_value<Enum>);
}

}	// namespace containers
