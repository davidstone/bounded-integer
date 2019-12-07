// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/reference_wrapper.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <operators/operators.hpp>

#include <iterator>
#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename LHS, typename RHS>
concept construct_subtractible = requires(LHS const lhs, RHS const rhs) {
	LHS(lhs - rhs);
};

} // namespace detail

struct repeat_n_sentinel {};

template<typename Size, typename Function>
struct repeat_n_iterator {
private:
	Size m_remaining;
	Function m_get_value;

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = std::remove_reference_t<decltype(std::declval<Function>()())>;
	using difference_type = decltype(std::declval<Size>() - std::declval<Size>());
	using pointer = value_type *;
	using reference = value_type &;
	
	constexpr repeat_n_iterator(Size const remaining, Function get_value):
		m_remaining(remaining),
		m_get_value(std::move(get_value))
	{
	}

	constexpr decltype(auto) operator*() const {
		return m_get_value();
	}
	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	// It is undefined behavior to compare iterators into different ranges
	friend constexpr auto operator<=>(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return lhs.m_remaining <=> rhs.m_remaining;
	}
	friend constexpr auto operator<=>(repeat_n_iterator const lhs, repeat_n_sentinel) {
		return lhs.m_remaining <=> 0_bi;
	}
	friend constexpr auto operator<=>(repeat_n_sentinel, repeat_n_iterator const rhs) {
		return 0_bi <=> rhs.m_remaining;
	}

	friend constexpr auto operator==(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return lhs.m_remaining == rhs.m_remaining;
	}
	friend constexpr auto operator==(repeat_n_iterator const lhs, repeat_n_sentinel) {
		return lhs.m_remaining == 0_bi;
	}
	friend constexpr auto operator==(repeat_n_sentinel, repeat_n_iterator const rhs) {
		return 0_bi == rhs.m_remaining;
	}

	template<bounded::integral Offset> requires(detail::construct_subtractible<Size, Offset>)
	friend constexpr auto operator+(repeat_n_iterator it, Offset const offset) -> repeat_n_iterator {
		return repeat_n_iterator(Size(it.m_remaining - offset), std::move(it).m_get_value);
	}
	friend constexpr auto operator-(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return rhs.m_remaining - lhs.m_remaining;
	}
	friend constexpr auto operator-(repeat_n_sentinel, repeat_n_iterator const rhs) {
		return rhs.m_remaining;
	}
};

template<typename Size, typename Function> requires(bounded::max_value<Size> == bounded::constant<0>)
constexpr auto & operator++(repeat_n_iterator<Size, Function> & it) {
	bounded::unreachable();
	return it;
}

namespace detail {

constexpr inline auto value_to_function = [](auto && value) {
	struct result {
		containers::reference_wrapper<std::remove_reference_t<decltype(value)>> m_ref;
		constexpr auto & operator()() const {
			return m_ref.get();
		}
	};
	return result{value};
};

} // namespace detail

template<typename Size, typename T>
struct repeat_n {
private:
	Size m_size;
	T m_value;

public:
	using size_type = Size;
	using value_type = T;

	using const_iterator = repeat_n_iterator<size_type, decltype(detail::value_to_function(std::declval<T const &>()))>;

	template<typename U>	
	constexpr repeat_n(size_type const size, U && value):
		m_size(size),
		m_value(BOUNDED_FORWARD(value))
	{
	}

	constexpr auto begin() const {
		return const_iterator(m_size, detail::value_to_function(m_value));
	}
	constexpr auto end() const {
		return repeat_n_sentinel{};
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
};

template<typename Size, typename T>
repeat_n(Size, T &&) -> repeat_n<bounded::integer<0, bounded::detail::builtin_max_value<Size>>, T>;

template<typename Size, typename T>
struct repeat_default_n_t {
private:
	Size m_size;

public:
	using size_type = Size;
	using value_type = T;
	using const_iterator = repeat_n_iterator<size_type, std::decay_t<decltype(bounded::construct_return<value_type>)>>;

	explicit constexpr repeat_default_n_t(size_type const size):
		m_size(size)
	{
	}

	constexpr auto begin() const {
		return const_iterator(m_size, bounded::construct_return<value_type>);
	}
	constexpr auto end() const {
		return repeat_n_sentinel{};
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
};

template<typename T, typename Size>
constexpr auto repeat_default_n(Size size) {
	return repeat_default_n_t<
		bounded::integer<0, bounded::detail::builtin_max_value<Size>>,
		T
	>(size);
}

}	// namespace containers
