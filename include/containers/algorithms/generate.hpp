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
		std::remove_const_t<Function>,
		decltype(std::reference_wrapper(std::declval<Function &>()))
	>;
	[[no_unique_address]] Size m_remaining;
	[[no_unique_address]] stored_function m_generator;

public:
	using iterator_category = std::input_iterator_tag;
	using reference = std::invoke_result_t<Function &>;
	using value_type = std::remove_cvref_t<reference>;
	using difference_type = decltype(std::declval<Size>() - std::declval<Size>());
	using pointer = std::remove_reference_t<reference> *;
	
	constexpr generate_n_iterator(Size const remaining, Function & generator):
		m_remaining(remaining),
		m_generator(generator)
	{
	}

	constexpr decltype(auto) operator*() const {
		return std::invoke(m_generator);
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

	friend constexpr auto operator+(generate_n_iterator it, bounded::constant_t<1>) -> generate_n_iterator {
		if constexpr (numeric_traits::max_value<Size> == bounded::constant<0>) {
			bounded::unreachable();
		} else {
			--it.m_remaining;
			return it;
		}
	}
};

// TODO: support generators with mutable state
template<typename Size, typename Function>
struct generate_n {
private:
	[[no_unique_address]] Size m_size;
	[[no_unique_address]] Function m_generator;

public:
	constexpr generate_n(Size const size, Function generator):
		m_size(size),
		m_generator(std::move(generator))
	{
	}

	constexpr auto begin() const requires std::is_invocable_v<Function const &> {
		return generate_n_iterator<bounded::integer<0, bounded::builtin_max_value<Size>>, Function const>(m_size, m_generator);
	}
	constexpr auto begin() {
		return generate_n_iterator<bounded::integer<0, bounded::builtin_max_value<Size>>, Function>(m_size, m_generator);
	}
	static constexpr auto end() {
		return generate_sentinel();
	}
	constexpr auto size() const {
		return m_size;
	}
};

} // namespace containers
