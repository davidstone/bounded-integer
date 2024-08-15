// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.array;

import containers.begin_end;
import containers.c_array;
import containers.data;
import containers.is_container;
import containers.maximum_array_size;
import containers.range_value_t;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, array_size_type<T> size, array_size_type<T>... sizes>
struct array;

template<typename T, array_size_type<T>... sizes>
struct array_value_type {
	using type = array<T, sizes...>;
};
template<typename T>
struct array_value_type<T> {
	using type = T;
};

struct monostate {
	friend auto operator<=>(monostate, monostate) = default;
};

template<std::size_t size>
struct array_trait {
	template<typename T>
	using type = T[size];
};

template<>
struct array_trait<0> {
	template<typename>
	using type = monostate;
};

template<typename T, std::size_t size>
using array_type = typename array_trait<size>::template type<T>;

export template<typename T, array_size_type<T> size_, array_size_type<T>... sizes>
struct array {
	using value_type = typename array_value_type<T, sizes...>::type;

	static constexpr auto size() {
		return bounded::constant<size_>;
	}
	constexpr auto data() const -> value_type const * {
		if constexpr (size() != 0) {
			return m_value;
		} else {
			return nullptr;
		}
	}
	constexpr auto data() -> value_type * {
		if constexpr (size() != 0) {
			return m_value;
		} else {
			return nullptr;
		}
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator<=>(array const &, array const &) = default;

	constexpr operator std::span<T const>() const & requires(sizeof...(sizes) == 0) {
		return std::span<T const>(m_value);
	}
	constexpr operator std::span<T>() & requires(sizeof...(sizes) == 0) {
		return std::span<T>(m_value);
	}
	constexpr operator std::span<T const, static_cast<std::size_t>(size_)>() const & requires(sizeof...(sizes) == 0) {
		return std::span<T const, static_cast<std::size_t>(size_)>(m_value);
	}
	constexpr operator std::span<T, static_cast<std::size_t>(size_)>() & requires(sizeof...(sizes) == 0) {
		return std::span<T, static_cast<std::size_t>(size_)>(m_value);
	}

	// Consider this private. It must be public for the class to be an
	// aggregate
	[[no_unique_address]] array_type<value_type, static_cast<std::size_t>(size_)> m_value;
};

template<typename... Args>
array(Args...) -> array<std::common_type_t<Args...>, bounded::constant<sizeof...(Args)>>;

template<typename T, std::size_t size>
array(c_array<T, size> &&) -> array<T, bounded::constant<size>>;

template<typename T, array_size_type<T>... sizes>
constexpr auto is_container<array<T, sizes...>> = true;

// Use the comma operator to expand the variadic pack
// Move the last element in if possible. Order of evaluation is well-defined for
// aggregate initialization, so there is no risk of copy-after-move
template<std::size_t... indexes>
constexpr auto make_array_n_impl(auto const size, auto && value, std::index_sequence<indexes...>) {
	return array<std::decay_t<decltype(value)>, size>({(void(indexes), value)..., OPERATORS_FORWARD(value)});
}

export template<auto size_>
constexpr auto make_array_n(bounded::constant_t<size_> size, auto && value) {
	if constexpr (size == 0_bi) {
		return array<std::decay_t<decltype(value)>, 0_bi>();
	} else {
		return ::containers::make_array_n_impl(
			size,
			OPERATORS_FORWARD(value),
			bounded::make_index_sequence(size - 1_bi)
		);
	}
}

template<std::size_t...indexes>
constexpr auto to_array_impl(auto && source, std::index_sequence<indexes...>) {
	return array({OPERATORS_FORWARD(source)[indexes]...});
}

export template<typename T, std::size_t size>
constexpr auto to_array(c_array<T, size> const & source) {
	return ::containers::to_array_impl(source, std::make_index_sequence<size>());
}
export template<typename T, std::size_t size>
constexpr auto to_array(c_array<T, size> && source) {
	return ::containers::to_array_impl(std::move(source), std::make_index_sequence<size>());
}
export template<typename T>
constexpr auto to_array(empty_c_array_parameter) {
	return array<T, 0_bi>();
}

export template<std::size_t index, typename T, array_size_type<T> size>
constexpr auto && get(array<T, size> const & a) {
	return a[bounded::constant<index>];
}
export template<std::size_t index, typename T, array_size_type<T> size>
constexpr auto && get(array<T, size> & a) {
	return a[bounded::constant<index>];
}
export template<std::size_t index, typename T, array_size_type<T> size>
constexpr auto && get(array<T, size> && a) {
	return std::move(a[bounded::constant<index>]);
}

} // namespace containers

template<typename T, containers::array_size_type<T> size_>
struct std::tuple_size<::containers::array<T, size_>> : std::integral_constant<std::size_t, static_cast<std::size_t>(size_)> {};

template<std::size_t index, typename T, containers::array_size_type<T> size>
struct std::tuple_element<index, ::containers::array<T, size>> {
	using type = T;
};

namespace {

constexpr auto size = 5_bi;
constexpr containers::array<int, size> a = {};
static_assert(a.size() == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");

static_assert(std::is_standard_layout_v<containers::array<int, 0_bi>>);
static_assert(std::is_trivial_v<containers::array<int, 0_bi>>);
static_assert(std::is_empty_v<containers::array<int, 0_bi>>);

template<typename T>
concept qualifiers_are_contiguous_ranges = 
	containers::contiguous_range<T const &> and
	containers::contiguous_range<T &>;

static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 0_bi>>);
static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 5_bi>>);

static_assert(containers::is_container<containers::array<int, 0_bi>>);
static_assert(containers::is_container<containers::array<int, 1_bi>>);

static_assert(bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int const>>);
static_assert(!bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int const>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int>>);

static_assert(bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int const, 3>>);
static_assert(!bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int, 3>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int const, 3>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int, 3>>);

constexpr auto array_n = containers::make_array_n(6_bi, 5);
static_assert(std::same_as<containers::range_value_t<decltype(array_n)>, int>, "Incorrect type from make_array_n.");
static_assert(array_n.size() == 6_bi, "Incorrect size from make_array_n.");
static_assert(array_n[3_bi] == 5, "Incorrect values from make_array_n.");

struct non_copyable {
	non_copyable() = default;
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
};

constexpr auto array_non_copyable = containers::make_array_n(1_bi, non_copyable());
static_assert(array_non_copyable.size() == 1_bi);

constexpr auto array_empty = containers::make_array_n(0_bi, 2);
static_assert(array_empty.size() == 0_bi);

constexpr auto array_non_copyable_empty = containers::make_array_n(0_bi, non_copyable());
static_assert(array_non_copyable_empty.size() == 0_bi);

constexpr auto large_size = 10000_bi;
constexpr auto large_array_n = containers::make_array_n(large_size, 0);
static_assert(large_array_n.size() == large_size);

} // namespace