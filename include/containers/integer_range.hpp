// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/operator_bracket.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Integer, typename Sentinel>
struct integer_range_iterator {
private:
	using storage_type = bounded::integer<
		bounded::detail::normalize<Integer::min().value()>,
		bounded::detail::normalize<Sentinel::max().value()>,
		typename Integer::overflow_policy
	>;
	storage_type m_value;

public:
	using value_type = bounded::integer<
		bounded::detail::normalize<storage_type::min().value()>,
		bounded::detail::normalize<bounded::max(storage_type::min(), storage_type::max() - bounded::constant<1>).value()>,
		typename Integer::overflow_policy
	>;
	using difference_type = decltype(std::declval<storage_type>() - std::declval<storage_type>());
	using pointer = value_type const *;
	using reference = value_type;
	using iterator_category = std::random_access_iterator_tag;

	integer_range_iterator() = default;
	explicit constexpr integer_range_iterator(storage_type const value) noexcept:
		m_value(value)
	{
	}
	
	friend constexpr auto operator-(integer_range_iterator const lhs, integer_range_iterator const rhs) noexcept {
		return lhs.m_value - rhs.m_value;
	}
	friend constexpr auto operator+(integer_range_iterator const lhs, difference_type const rhs) noexcept(
		std::is_nothrow_constructible_v<storage_type, decltype(std::declval<storage_type>() + rhs)>
	) {
		return integer_range_iterator(storage_type(lhs.m_value + rhs));
	}
	
	friend constexpr auto compare(integer_range_iterator const lhs, integer_range_iterator const rhs) noexcept {
		return compare(lhs.m_value, rhs.m_value);
	}

	// It is undefined behavior to dereference a past-the-end iterator.
	//
	// Note that this returns a value, not a reference.
	constexpr auto operator*() const BOUNDED_NOEXCEPT_VALUE(
		value_type(m_value)
	)
	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(integer_range_iterator)
};

template<typename Integer, typename Sentinel>
constexpr auto operator+(typename integer_range_iterator<Integer, Sentinel>::difference_type const lhs, integer_range_iterator<Integer, Sentinel> const rhs) {
	return rhs + lhs;
}

}	// namespace detail

template<typename Integer, typename Sentinel = Integer>
struct integer_range {
	static_assert(bounded::is_bounded_integer<Integer>);
	static_assert(bounded::is_bounded_integer<Sentinel>);
	static_assert(Sentinel::max() >= Integer::min(), "Cannot construct inverted integer ranges.");

	using iterator = detail::integer_range_iterator<Integer, Sentinel>;
	using const_iterator = iterator;

	using value_type = typename iterator::value_type;
	using size_type = bounded::integer<0, bounded::detail::normalize<iterator::difference_type::max().value()>>;

	constexpr integer_range(Integer const first, Sentinel const last) noexcept:
		m_begin(first),
		m_end(last)
	{
	}
	template<typename Size>
	constexpr integer_range(Size const size) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer_range(bounded::constant<0>, size)
	) {
	}

	friend constexpr auto begin(integer_range const range) noexcept {
		return iterator(range.m_begin);
	}
	friend constexpr auto end(integer_range const range) noexcept {
		return iterator(range.m_end);
	}

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(integer_range)

private:
	Integer m_begin;
	Sentinel m_end;
};

template<typename Integer, typename Sentinel>
integer_range(Integer, Sentinel) -> integer_range<
	bounded::integer<
		bounded::detail::normalize<std::numeric_limits<Integer>::min().value()>,
		bounded::detail::normalize<std::numeric_limits<Sentinel>::max().value()>
	>,
	Sentinel
>;

template<typename Size>
integer_range(Size) -> integer_range<bounded::integer<0, bounded::detail::normalize<Size::max().value()>>, Size>;


template<typename Enum, BOUNDED_REQUIRES(std::is_enum_v<Enum>)>
constexpr auto enum_range(Enum last = static_cast<Enum>(std::numeric_limits<Enum>::max())) {
	auto const irange = integer_range(bounded::integer(last));
	constexpr struct {
		constexpr auto operator()(decltype(begin(irange)) const it) const {
			return static_cast<Enum>(*it);
		}
	} function;
	return containers::adapt_range(irange, function);
}

}	// namespace containers
