// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/operator_bracket.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Integer, typename Sentinel, typename Step>
struct integer_range_iterator {
private:
	using storage_type = bounded::integer<
		bounded::detail::normalize<Integer::min().value()>,
		bounded::detail::normalize<Sentinel::max().value()>,
		typename Integer::overflow_policy
	>;
	storage_type m_value;
	Step m_step;

public:
	using value_type = bounded::integer<
		bounded::detail::normalize<storage_type::min().value()>,
		bounded::detail::normalize<bounded::max(storage_type::min(), storage_type::max() - Step::min()).value()>,
		typename Integer::overflow_policy
	>;
	using difference_type = decltype(std::declval<storage_type>() - std::declval<storage_type>());
	using pointer = value_type const *;
	using reference = value_type;
	using iterator_category = std::random_access_iterator_tag;

	integer_range_iterator() = default;
	explicit constexpr integer_range_iterator(storage_type const value, Step const step) noexcept:
		m_value(value),
		m_step(step)
	{
	}
	
	friend constexpr auto operator-(integer_range_iterator const lhs, integer_range_iterator const rhs) noexcept {
		assert(lhs.m_step == rhs.m_step);
		return (lhs.m_value - rhs.m_value) / lhs.m_step;
	}
	friend constexpr auto operator+(integer_range_iterator const lhs, difference_type const rhs) noexcept(
		std::is_nothrow_constructible_v<storage_type, decltype(std::declval<storage_type>() + rhs * std::declval<Step>())>
	) {
		return integer_range_iterator(storage_type(lhs.m_value + rhs * lhs.m_step), lhs.m_step);
	}
	
	friend constexpr auto compare(integer_range_iterator const lhs, integer_range_iterator const rhs) noexcept {
		return compare(lhs.m_value, rhs.m_value);
	}

	friend constexpr auto operator==(integer_range_iterator const lhs, integer_range_iterator const rhs) noexcept {
		return lhs.m_value == rhs.m_value;
	}

	// It is undefined behavior to dereference a past-the-end iterator.
	//
	// Note that this returns a value, not a reference.
	constexpr auto operator*() const BOUNDED_NOEXCEPT_VALUE(
		value_type(m_value)
	)
	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(integer_range_iterator)
};

template<typename Integer, typename Sentinel, typename Step>
constexpr auto operator+(typename integer_range_iterator<Integer, Sentinel, Step>::difference_type const lhs, integer_range_iterator<Integer, Sentinel, Step> const rhs) {
	return rhs + lhs;
}

}	// namespace detail

template<typename Integer, typename Sentinel = Integer, typename Step = bounded::constant_t<1>>
struct integer_range {
	static_assert(bounded::is_bounded_integer<Integer>);
	static_assert(bounded::is_bounded_integer<Sentinel>);
	static_assert(Sentinel::max() >= Integer::min(), "Cannot construct inverted integer ranges.");

	using iterator = detail::integer_range_iterator<Integer, Sentinel, Step>;
	using const_iterator = iterator;

	using value_type = typename iterator::value_type;
	using size_type = bounded::integer<0, bounded::detail::normalize<iterator::difference_type::max().value()>>;
	
	// If `last` is not reachable by adding `step` to `first` some number of
	// times, the behavior is undefined.
	constexpr integer_range(Integer const first, Sentinel const last, Step const step) noexcept:
		m_begin(first),
		m_end(last),
		m_step(step)
	{
	}

	constexpr integer_range(Integer const first, Sentinel const last) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer_range(first, last, bounded::constant<1>)
	) {
	}
	template<typename Size>
	constexpr integer_range(Size const size) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer_range(bounded::constant<0>, size)
	) {
	}

	friend constexpr auto begin(integer_range const range) noexcept {
		return iterator(range.m_begin, range.m_step);
	}
	friend constexpr auto end(integer_range const range) noexcept {
		return iterator(range.m_end, range.m_step);
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(integer_range)

private:
	Integer m_begin;
	Sentinel m_end;
	Step m_step;
};

template<typename Size>
integer_range(Size) -> integer_range<bounded::constant_t<0>, Size>;

template<typename Integer, typename Sentinel, typename Step = bounded::constant_t<1>>
constexpr auto inclusive_integer_range(Integer const first, Sentinel const last, Step const step = bounded::constant<1>) BOUNDED_NOEXCEPT_VALUE(
	integer_range(first, last + step, step)
)

template<typename Enum, BOUNDED_REQUIRES(std::is_enum_v<Enum>)>
constexpr auto enum_range(Enum last = static_cast<Enum>(std::numeric_limits<Enum>::max())) {
	return containers::transform(
		inclusive_integer_range(bounded::constant<0>, bounded::integer(last)),
		[](auto e) { return static_cast<Enum>(e); }
	);
}

}	// namespace containers
