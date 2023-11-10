// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>

export module containers.algorithms.generate;

import containers.algorithms.compare;
import containers.array;
import containers.common_iterator_functions;
import containers.index_type;
import containers.offset_type;
import containers.stored_function;

import bounded;
import numeric_traits;
import std_module;

// TODO: Implement `generate`, then `generate_n(f, n)` is `take(generate(f), n)`

using namespace bounded::literal;

namespace containers {

template<typename LHS, typename RHS>
concept construct_subtractable = requires(LHS const lhs, RHS const rhs) {
	LHS(lhs - rhs);
};

struct generate_sentinel {};

template<typename Offset, typename Function>
struct generate_n_iterator {
private:
	[[no_unique_address]] Offset m_remaining;
	[[no_unique_address]] stored_function<Function> m_generator;

public:
	using iterator_category = std::input_iterator_tag;
	using difference_type = decltype(bounded::declval<Offset>() - bounded::declval<Offset>());
	
	constexpr generate_n_iterator(Offset const remaining, Function & generator):
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
		if constexpr (numeric_traits::max_value<Offset> == 0_bi) {
			std::unreachable();
		} else {
			--it.m_remaining;
			return it;
		}
	}
};

export template<typename Size, typename Function>
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

	constexpr auto begin() const requires std::invocable<Function const &> {
		return generate_n_iterator(to_offset_type<Size>(m_size), m_generator);
	}
	constexpr auto begin() {
		return generate_n_iterator(to_offset_type<Size>(m_size), m_generator);
	}
	static constexpr auto end() {
		return generate_sentinel();
	}
	constexpr auto size() const {
		return m_size;
	}
};

template<typename T>
using generate_index_type = to_index_type<std::conditional_t<
	std::same_as<T, bounded::constant_t<0>>,
	bounded::constant_t<1>,
	T
>>;

template<typename Function, typename Size>
concept index_invocable = std::invocable<Function const &, generate_index_type<Size>>;

template<typename Offset, typename Function>
struct indexed_generate_n_iterator {
private:
	[[no_unique_address]] Offset m_offset;
	[[no_unique_address]] stored_function<Function const> m_generator;

public:
	using difference_type = decltype(bounded::declval<Offset>() - bounded::declval<Offset>());
	
	constexpr indexed_generate_n_iterator(Offset const remaining, Function const & generator):
		m_offset(remaining),
		m_generator(generator)
	{
	}

	constexpr decltype(auto) operator*() const {
		return std::invoke(m_generator, bounded::assume_in_range<generate_index_type<Offset>>(m_offset));
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator<=>(indexed_generate_n_iterator const lhs, indexed_generate_n_iterator const rhs) {
		return lhs.m_offset <=> rhs.m_offset;
	}
	friend constexpr auto operator==(indexed_generate_n_iterator const lhs, indexed_generate_n_iterator const rhs) -> bool {
		return lhs.m_offset == rhs.m_offset;
	}

	friend constexpr auto operator+(indexed_generate_n_iterator const it, difference_type const difference) -> indexed_generate_n_iterator {
		return {bounded::assume_in_range<Offset>(it.m_offset + difference), it.m_generator};
	}
	friend constexpr auto operator+(indexed_generate_n_iterator const it, bounded::constant_t<1>) -> indexed_generate_n_iterator requires(bounded::builtin_integer<Offset> or numeric_traits::max_value<Offset> == 0_bi) {
		if constexpr (bounded::builtin_integer<Offset>) {
			return it + 1;
		} else {
			std::unreachable();
		}
	}
	friend constexpr auto operator+(indexed_generate_n_iterator const it, bounded::constant_t<-1>) -> indexed_generate_n_iterator requires(bounded::builtin_integer<Offset> or numeric_traits::max_value<Offset> == 0_bi) {
		if constexpr (bounded::builtin_integer<Offset>) {
			return it - 1;
		} else {
			std::unreachable();
		}
	}

	friend constexpr auto operator-(indexed_generate_n_iterator const lhs, indexed_generate_n_iterator const rhs) -> difference_type {
		return lhs.m_offset - rhs.m_offset;
	}
};

export template<typename Size, index_invocable<Size> Function>
struct indexed_generate_n {
private:
	[[no_unique_address]] Size m_size;
	[[no_unique_address]] Function m_generator;
public:
	constexpr indexed_generate_n(Size const size, Function generator):
		m_size(size),
		m_generator(std::move(generator))
	{
	}

	constexpr auto begin() const {
		return indexed_generate_n_iterator(to_offset_type<Size>(0_bi), m_generator);
	}
	constexpr auto size() const {
		return m_size;
	}
};

} // namespace containers

static_assert(containers::equal(containers::generate_n(0_bi, bounded::value_to_function(5)), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::generate_n(1_bi, bounded::value_to_function(5)), containers::array{5}));
static_assert(containers::equal(containers::generate_n(2_bi, bounded::value_to_function(5)), containers::array{5, 5}));

static_assert(containers::equal(containers::indexed_generate_n(0_bi, bounded::copy), containers::array<bounded::constant_t<0>, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1_bi, bounded::copy), containers::array{0_bi}));
static_assert(containers::equal(containers::indexed_generate_n(2_bi, bounded::copy), containers::array{0_bi, 1_bi}));

static_assert(containers::equal(containers::indexed_generate_n(0, bounded::copy), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::indexed_generate_n(1, bounded::copy), containers::array{0}));
static_assert(containers::equal(containers::indexed_generate_n(2, bounded::copy), containers::array{0, 1}));