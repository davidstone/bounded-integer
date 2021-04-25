// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/begin_end.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/forward.hpp>
#include <operators/operators.hpp>

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

// TODO: Implement `generate`, then `generate_n(f, n)` is `take(generate(f), n)`

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename LHS, typename RHS>
concept construct_subtractable = requires(LHS const lhs, RHS const rhs) {
	LHS(lhs - rhs);
};

} // namespace detail

struct generate_sentinel {};

template<typename Size, typename Function>
struct generate_n_iterator {
private:
	using stored_function = std::conditional_t<
		std::is_empty_v<Function> and std::is_trivially_copyable_v<Function>,
		Function,
		decltype(std::reference_wrapper(std::declval<Function const &>()))
	>;
	[[no_unique_address]] Size m_remaining;
	[[no_unique_address]] stored_function m_generator;

public:
	using iterator_category = std::random_access_iterator_tag;
	using reference = decltype(std::declval<Function>()());
	using value_type = std::remove_cvref_t<reference>;
	using difference_type = decltype(std::declval<Size>() - std::declval<Size>());
	using pointer = std::remove_reference_t<reference> *;
	
	constexpr generate_n_iterator(Size const remaining, Function const & generator):
		m_remaining(remaining),
		m_generator(generator)
	{
	}

	constexpr decltype(auto) operator*() const {
		return m_generator();
	}
	OPERATORS_ARROW_DEFINITIONS

	// It is undefined behavior to compare iterators into different ranges
	friend constexpr auto operator<=>(generate_n_iterator const lhs, generate_n_iterator const rhs) {
		return lhs.m_remaining <=> rhs.m_remaining;
	}
	friend constexpr auto operator<=>(generate_n_iterator const lhs, generate_sentinel) {
		return lhs.m_remaining <=> 0_bi;
	}

	friend constexpr auto operator==(generate_n_iterator const lhs, generate_n_iterator const rhs) -> bool {
		return lhs.m_remaining == rhs.m_remaining;
	}
	friend constexpr auto operator==(generate_n_iterator const lhs, generate_sentinel) -> bool {
		return lhs.m_remaining == 0_bi;
	}

	template<bounded::integral Offset> requires(detail::construct_subtractable<Size, Offset>)
	friend constexpr auto operator+(generate_n_iterator it, Offset const offset) -> generate_n_iterator {
		return generate_n_iterator(Size(it.m_remaining - offset), std::move(it).m_generator);
	}
	friend constexpr auto operator-(generate_n_iterator const lhs, generate_n_iterator const rhs) {
		return rhs.m_remaining - lhs.m_remaining;
	}
	friend constexpr auto operator-(generate_sentinel, generate_n_iterator const rhs) {
		return rhs.m_remaining;
	}
	friend constexpr auto & operator++(generate_n_iterator & it) requires(numeric_traits::max_value<Size> == bounded::constant<0>) {
		bounded::unreachable();
		return it;
	}
};

// TODO: support generators with mutable state
template<typename Size, typename Function>
struct generate_n {
private:
	[[no_unique_address]] Size m_size;
	[[no_unique_address]] Function m_generator;

public:
	using size_type = Size;
	using value_type = std::invoke_result_t<Function const &>;

	using const_iterator = generate_n_iterator<bounded::integer<0, bounded::builtin_max_value<size_type>>, Function>;

	constexpr generate_n(Size const size, Function generator):
		m_size(size),
		m_generator(std::move(generator))
	{
	}

	constexpr auto begin() const {
		return const_iterator(m_size, m_generator);
	}
	static constexpr auto end() {
		return generate_sentinel();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
};

} // namespace containers
