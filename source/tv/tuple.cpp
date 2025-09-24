// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.tuple;

import tv.nth_type;

import bounded;
import std_module;

using namespace bounded::literal;

namespace tv {

// index is the index of this value within the tuple that contains it
template<std::size_t index, typename T>
struct tuple_value {
	tuple_value() = default;

	constexpr explicit tuple_value(bounded::lazy_init_t, bounded::construct_function_for<T> auto function):
		m_value(std::move(function)())
	{
	}

	constexpr explicit tuple_value(bounded::convertible_to<T> auto && arg):
		m_value(OPERATORS_FORWARD(arg))
	{
	}

	constexpr auto && operator[](bounded::constant_t<bounded::normalize<index>>) const & {
		return m_value;
	}
	constexpr auto && operator[](bounded::constant_t<bounded::normalize<index>>) & {
		return m_value;
	}
	constexpr auto && operator[](bounded::constant_t<bounded::normalize<index>>) && {
		// This should not be std::move because that would return an rvalue
		// reference even if T is an lvalue reference type.
		return static_cast<T &&>(m_value);
	}

	constexpr auto && operator[](bounded::type_t<T>) const & {
		return m_value;
	}
	constexpr auto && operator[](bounded::type_t<T>) & {
		return m_value;
	}
	constexpr auto && operator[](bounded::type_t<T>) && {
		// This should not be std::move because that would return an rvalue
		// reference even if T is an lvalue reference type.
		return static_cast<T &&>(m_value);
	}

private:
	// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if defined __GNUC__ and !defined __clang__
	T m_value;
#else
	[[no_unique_address]] T m_value;
#endif
};

template<std::size_t index>
struct tuple_value<index, void> {
	tuple_value() = default;

	constexpr explicit tuple_value(bounded::lazy_init_t, bounded::construct_function_for<void> auto function) {
		std::move(function)();
	}

	constexpr void operator[](bounded::constant_t<bounded::normalize<index>>) const {
	}
	constexpr void operator[](bounded::type_t<void>) const {
	}
};

struct unusable {
	unusable() = delete;
};

template<typename Indexes, typename... Types>
struct tuple_impl;

template<std::size_t... indexes, typename... Types>
struct tuple_impl<std::index_sequence<indexes...>, Types...> : tuple_value<indexes, Types>... {

	tuple_impl() = default;

	constexpr tuple_impl(bounded::convertible_to<Types> auto && ... args):
		tuple_value<indexes, Types>(OPERATORS_FORWARD(args))...
	{
	}

	constexpr tuple_impl(bounded::lazy_init_t, bounded::construct_function_for<Types> auto ... functions):
		tuple_value<indexes, Types>(bounded::lazy_init, std::move(functions))...
	{
	}

	using tuple_value<indexes, Types>::operator[]...;

protected:
	// Add in an overload that is guaranteed to exist so the using operator[]
	// in tuple is well-formed even for empty tuples
	void operator[](unusable) = delete;
};

template<typename... Types>
using tuple_impl_t = tuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...>;


export template<typename... Types>
struct tuple : private tuple_impl_t<Types...> {
	using tuple_impl_t<Types...>::tuple_impl_t;
	using tuple_impl_t<Types...>::operator[];

	template<std::size_t index>
	constexpr auto get() const & -> auto && {
		return (*this)[bounded::constant<index>];
	}
	template<std::size_t index>
	constexpr auto get() & -> auto && {
		return (*this)[bounded::constant<index>];
	}
	template<std::size_t index>
	constexpr auto get() && -> auto && {
		return std::move(*this)[bounded::constant<index>];
	}

	template<bounded::ordered<Types>... RHSTypes>
	friend constexpr auto operator<=>(tuple const & lhs, tuple<RHSTypes...> const & rhs) {
		auto const [...indexes] = bounded::index_sequence_struct<sizeof...(Types)>();
		auto cmp = std::strong_ordering::equal;
		void((... or ((cmp = lhs[indexes] <=> rhs[indexes]), cmp != 0)));
		return cmp;
	}

	template<bounded::equality_comparable<Types>... RHSTypes>
	friend constexpr auto operator==(tuple const & lhs, tuple<RHSTypes...> const & rhs) -> bool {
		auto const [...indexes] = bounded::index_sequence_struct<sizeof...(Types)>();
		return (... and (lhs[indexes] == rhs[indexes]));
	}
};

template<typename... Ts>
tuple(Ts && ...) -> tuple<std::decay_t<Ts>...>;


template<typename T>
constexpr auto is_tuple = false;

template<typename... Types>
constexpr auto is_tuple<tuple<Types...>> = true;

// TODO: tuple_like to be based on requirements, not exact match
export template<typename T>
concept tuple_like = is_tuple<std::decay_t<T>>;


// TODO: unwrap reference_wrapper?
export constexpr auto make_tuple = [](auto && ... args) {
	return tuple<std::decay_t<decltype(args)>...>(OPERATORS_FORWARD(args)...);
};

export constexpr auto tie = [](auto && ... args) {
	return tuple<decltype(args)...>(OPERATORS_FORWARD(args)...);
};

template<std::size_t index, typename Tuple>
struct tuple_element_c;

template<std::size_t index, typename... Types>
struct tuple_element_c<index, tuple<Types...>> {
	using type = nth_type<index, Types...>;
};

export template<std::size_t index, typename Tuple>
using tuple_element = typename tuple_element_c<index, Tuple>::type;


export template<std::size_t index>
constexpr auto && get(tuple_like auto && t) {
	return OPERATORS_FORWARD(t)[bounded::constant<index>];
}

}	// namespace tv

template<typename... Ts>
class std::tuple_size<tv::tuple<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<std::size_t index, typename... Ts>
class std::tuple_element<index, tv::tuple<Ts...>> : public tv::tuple_element_c<index, tv::tuple<Ts...>> {};
