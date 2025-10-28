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
import containers.is_container;
import containers.maximum_array_size;

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

export template<typename T, array_size_type<T> size_, array_size_type<T>... sizes>
struct array {
	using value_type = typename array_value_type<T, sizes...>::type;

	static constexpr auto size() {
		return bounded::constant<size_>;
	}
	constexpr auto data() const -> value_type const * {
		return m_value;
	}
	constexpr auto data() -> value_type * {
		return m_value;
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
	c_array<value_type, static_cast<std::size_t>(size_)> m_value;
};

template<typename T, array_size_type<T> size_> requires(size_ == 0_bi)
struct array<T, size_> {
private:
	struct monostate {
		friend auto operator<=>(monostate, monostate) = default;
	};
public:
	using value_type = T;

	static constexpr auto size() -> bounded::constant_t<0> {
		return 0_bi;
	}
	constexpr auto data() const -> value_type const * {
		return nullptr;
	}
	constexpr auto data() -> value_type * {
		return nullptr;
	}

	friend auto operator<=>(array, array) = default;

	constexpr operator std::span<T const>() const {
		return std::span<T const>(data(), 0);
	}
	constexpr operator std::span<T>() {
		return std::span<T const>(data(), 0);
	}
	constexpr operator std::span<T const, 0>() const {
		return std::span<T const, 0>(data(), 0);
	}
	constexpr operator std::span<T, 0>() {
		return std::span<T const, 0>(data(), 0);
	}

	// Consider this private. It must be public for the class to be an
	// aggregate
	[[no_unique_address]] monostate m_value;
};

template<typename... Args>
array(Args...) -> array<std::common_type_t<Args...>, bounded::constant<sizeof...(Args)>>;

template<typename T, std::size_t size>
array(c_array<T, size> &&) -> array<T, bounded::constant<size>>;

template<typename T, array_size_type<T>... sizes>
constexpr auto is_container<array<T, sizes...>> = true;

export template<auto size_>
constexpr auto make_array_n(bounded::constant_t<size_> size, auto && value) {
	using result_t = array<std::decay_t<decltype(value)>, size>;
	if constexpr (size == 0_bi) {
		return result_t();
	} else {
		auto [...indexes] = bounded::index_sequence_struct<size_ - 1>();
		// Use the comma operator to expand the variadic pack. Move the last
		// element if possible. Order of evaluation is well-defined for
		// aggregate initialization, so there is no risk of copy-after-move
		return result_t({(void(indexes), value)..., OPERATORS_FORWARD(value)});
	}
}

export template<typename T, std::size_t size>
constexpr auto to_array(c_array<T, size> const & source) {
	auto [...indexes] = bounded::index_sequence_struct<size>();
	return array({source[std::size_t(indexes)]...});
}
export template<typename T, std::size_t size>
constexpr auto to_array(c_array<T, size> && source) {
	auto [...indexes] = bounded::index_sequence_struct<size>();
	return array({std::move(source)[std::size_t(indexes)]...});
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
