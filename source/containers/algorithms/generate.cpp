// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>

export module containers.algorithms.generate;

export import containers.common_iterator_functions;
import containers.index_type;
import containers.offset_type;
import containers.stored_function;

import bounded;
import numeric_traits;
import std_module;
import tv;

using namespace bounded::literal;

namespace containers {

template<typename Generator>
struct generate_iterator {
	using iterator_category = std::input_iterator_tag;
	// TODO: ???
	using difference_type = decltype(bounded::integer(std::int64_t()));
	
	explicit constexpr generate_iterator(Generator & generator):
		m_generator(generator)
	{
	}
	generate_iterator(generate_iterator &&) = default;
	generate_iterator(generate_iterator const &) = delete;
	auto operator=(generate_iterator &&) & -> generate_iterator & = default;
	auto operator=(generate_iterator const &) & -> generate_iterator & = delete;

	constexpr auto operator*() const -> decltype(auto) {
		return *std::move(m_generator.get().m_cached);
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator==(generate_iterator const & lhs, std::default_sentinel_t) -> bool {
		return !lhs.m_generator.get().m_cached;
	}

	friend constexpr auto operator+(generate_iterator it, bounded::constant_t<1>) -> generate_iterator {
		it.m_generator.get().populate_cache();
		return it;
	}
private:
	std::reference_wrapper<Generator> m_generator;
};
	
export template<std::invocable Function>
struct generate {
	explicit constexpr generate(Function function):
		m_function(std::move(function))
	{
	}

	constexpr auto begin() {
		populate_cache();
		return generate_iterator<generate<Function>>(*this);
	}
	static constexpr auto end() -> std::default_sentinel_t {
		return std::default_sentinel;
	}
private:
	friend struct generate_iterator<generate>;
	constexpr auto populate_cache() {
		m_cached = std::invoke(m_function);
	}
	[[no_unique_address]] Function m_function;
	std::invoke_result_t<Function &> m_cached;
};


template<typename LHS, typename RHS>
concept construct_subtractable = requires(LHS const lhs, RHS const rhs) {
	LHS(lhs - rhs);
};

template<typename Offset>
constexpr auto get_generator(auto && generator) -> auto && {
	if constexpr (numeric_traits::max_value<Offset> <= 1_bi) {
		return std::move(generator);
	} else {
		return generator;
	}
}

template<typename T>
using generate_index_type = to_index_type<std::conditional_t<
	std::same_as<T, bounded::constant_t<0>>,
	bounded::constant_t<1>,
	T
>>;

template<typename Offset, typename Function>
struct generate_n_iterator {
	using iterator_category = std::input_iterator_tag;
	using difference_type = decltype(bounded::declval<Offset>() - bounded::declval<Offset>());
	
	constexpr generate_n_iterator(Offset const remaining, Function & generator):
		m_remaining(remaining),
		m_generator(generator)
	{
	}

	generate_n_iterator(generate_n_iterator &&) = default;
	generate_n_iterator(generate_n_iterator const &) = delete;
	auto operator=(generate_n_iterator &&) & -> generate_n_iterator & = default;
	auto operator=(generate_n_iterator const &) & -> generate_n_iterator & = delete;

	constexpr auto operator*() const -> decltype(auto) {
		return std::invoke(::containers::get_generator<Offset>(m_generator.get()));
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator<=>(generate_n_iterator const & lhs, std::default_sentinel_t) {
		return lhs.m_remaining <=> 0_bi;
	}
	friend constexpr auto operator==(generate_n_iterator const & lhs, std::default_sentinel_t) -> bool {
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
private:
	[[no_unique_address]] Offset m_remaining;
	[[no_unique_address]] stored_function<Function> m_generator;
};

template<typename Offset, typename Function, bool indexed>
struct idempotent_generate_n_iterator {
	using difference_type = decltype(bounded::declval<Offset>() - bounded::declval<Offset>());

	constexpr idempotent_generate_n_iterator(
		Offset const remaining,
		Function const & generator
	):
		m_offset(remaining),
		m_generator(generator)
	{
	}

	constexpr auto operator*() const -> decltype(auto) {
		if constexpr (indexed) {
			return std::invoke(
				m_generator,
				bounded::assume_in_range<generate_index_type<Offset>>(m_offset)
			);
		} else {
			return std::invoke(::containers::get_generator<Offset>(m_generator.get()));
		}
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator<=>(idempotent_generate_n_iterator const lhs, idempotent_generate_n_iterator const rhs) {
		return lhs.m_offset <=> rhs.m_offset;
	}
	friend constexpr auto operator==(idempotent_generate_n_iterator const lhs, idempotent_generate_n_iterator const rhs) -> bool {
		return lhs.m_offset == rhs.m_offset;
	}

	friend constexpr auto operator+(idempotent_generate_n_iterator const it, difference_type const difference) -> idempotent_generate_n_iterator {
		return idempotent_generate_n_iterator(
			bounded::assume_in_range<Offset>(it.m_offset + difference),
			it.m_generator.get()
		);
	}
	friend constexpr auto operator+(idempotent_generate_n_iterator const it, bounded::constant_t<1>) -> idempotent_generate_n_iterator requires(bounded::builtin_integer<Offset> or numeric_traits::max_value<Offset> == 0_bi) {
		if constexpr (bounded::builtin_integer<Offset>) {
			return it + 1;
		} else {
			std::unreachable();
		}
	}
	friend constexpr auto operator+(idempotent_generate_n_iterator const it, bounded::constant_t<-1>) -> idempotent_generate_n_iterator requires(bounded::builtin_integer<Offset> or numeric_traits::max_value<Offset> == 0_bi) {
		if constexpr (bounded::builtin_integer<Offset>) {
			return it - 1;
		} else {
			std::unreachable();
		}
	}

	friend constexpr auto operator-(idempotent_generate_n_iterator const lhs, idempotent_generate_n_iterator const rhs) -> difference_type {
		return lhs.m_offset - rhs.m_offset;
	}
private:
	using function_t = std::conditional_t<
		indexed,
		stored_function<Function const, generate_index_type<Offset>>,
		stored_function<Function const>
	>;
	[[no_unique_address]] Offset m_offset;
	[[no_unique_address]] function_t m_generator;
};

template<typename Function>
concept mutable_invocable = std::invocable<Function &>;

export template<typename Size, mutable_invocable Function>
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
		return generate_n_iterator(
			to_offset_type<Size>(m_size),
			m_generator
		);
	}
	constexpr auto begin() {
		return generate_n_iterator(
			to_offset_type<Size>(m_size),
			m_generator
		);
	}
	static constexpr auto end() -> std::default_sentinel_t {
		return std::default_sentinel;
	}
	constexpr auto size() const -> Size {
		return m_size;
	}
};

template<typename Function>
concept idempotent_invocable = std::invocable<Function const &>;

export template<typename Size, idempotent_invocable Function>
struct generate_n_idempotent {
	static_assert(!numeric_traits::unsigned_builtin<Size>);
private:
	[[no_unique_address]] Size m_size;
	[[no_unique_address]] Function m_generator;

public:
	constexpr generate_n_idempotent(Size const size, Function generator):
		m_size(size),
		m_generator(std::move(generator))
	{
	}

	constexpr auto begin() const -> idempotent_generate_n_iterator<to_offset_type<Size>, Function, false> {
		return {to_offset_type<Size>(0_bi), m_generator};
	}
	constexpr auto size() const -> Size {
		return m_size;
	}
};

template<typename Function, typename Size>
concept index_invocable = std::invocable<Function const &, generate_index_type<Size>>;

export template<typename Size, index_invocable<Size> Function>
struct indexed_generate_n {
	static_assert(!numeric_traits::unsigned_builtin<Size>);
private:
	[[no_unique_address]] Size m_size;
	[[no_unique_address]] Function m_generator;
public:
	constexpr indexed_generate_n(Size const size, Function generator):
		m_size(size),
		m_generator(std::move(generator))
	{
	}

	constexpr auto begin() const -> idempotent_generate_n_iterator<to_offset_type<Size>, Function, true> {
		return {to_offset_type<Size>(0_bi), m_generator};
	}
	constexpr auto size() const -> Size {
		return m_size;
	}
};

template<typename Function>
struct generate_until_sentinel {
	explicit constexpr generate_until_sentinel(Function & is_done_):
		m_is_done(is_done_)
	{
	}
	constexpr auto is_done() const -> bool {
		return std::invoke(m_is_done.get());
	}
private:
	[[no_unique_address]] stored_function<Function> m_is_done;
};

} // namespace containers
