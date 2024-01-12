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

template<std::size_t... indexes>
constexpr auto compare_impl(auto const & lhs, auto const & rhs, std::index_sequence<indexes...>) {
	auto cmp = std::strong_ordering::equal;
	void((... or ((cmp = lhs[bounded::constant<indexes>] <=> rhs[bounded::constant<indexes>]), cmp != 0)));
	return cmp;
}

template<std::size_t... indexes>
constexpr auto equal_impl(auto const & lhs, auto const & rhs, std::index_sequence<indexes...>) {
	return (... and (lhs[bounded::constant<indexes>] == rhs[bounded::constant<indexes>]));
}


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
	struct unusable {
		unusable() = delete;
	};
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
		return ::tv::compare_impl(lhs, rhs, bounded::make_index_sequence(bounded::constant<sizeof...(Types)>));
	}

	template<bounded::equality_comparable<Types>... RHSTypes>
	friend constexpr auto operator==(tuple const & lhs, tuple<RHSTypes...> const & rhs) -> bool {
		return ::tv::equal_impl(lhs, rhs, bounded::make_index_sequence(bounded::constant<sizeof...(Types)>));
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

template<typename Tuple>
struct tuple_size_c;

template<typename... Types>
struct tuple_size_c<tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

export template<typename Tuple>
constexpr auto tuple_size = bounded::constant<tuple_size_c<std::decay_t<Tuple>>::value>;


template<std::size_t index, typename Tuple>
struct tuple_element_c;

template<std::size_t index, typename... Types>
struct tuple_element_c<index, tuple<Types...>> {
	using type = nth_type<index, Types...>;
};

export template<std::size_t index, typename Tuple>
using tuple_element = typename tuple_element_c<index, Tuple>::type;


template<typename Indexes, typename Tuple>
struct indexed_tuple;

template<std::size_t... indexes, typename Tuple>
struct indexed_tuple<std::index_sequence<indexes...>, Tuple> {
	Tuple && tuple;
};

template<typename Tuple>
indexed_tuple(Tuple &&) -> indexed_tuple<std::make_index_sequence<tuple_size<Tuple>.value()>, Tuple>;

struct tuple_cat_t {
private:
	template<std::size_t... first_indexes, typename First, std::size_t... second_indexes, typename Second>
	static constexpr auto cat_impl(
		indexed_tuple<std::index_sequence<first_indexes...>, First> first,
		indexed_tuple<std::index_sequence<second_indexes...>, Second> second,
		auto && ... tail
	) {
		return operator()(
			tuple<
				tuple_element<first_indexes, First>...,
				tuple_element<second_indexes, Second>...
			>(
				OPERATORS_FORWARD(first).tuple[bounded::constant<first_indexes>]...,
				OPERATORS_FORWARD(second).tuple[bounded::constant<second_indexes>]...
			),
			OPERATORS_FORWARD(tail).tuple...
		);
	}

public:
	template<typename... Tuples> requires(... and bounded::constructible_from<std::decay_t<Tuples>, Tuples &&>)
	static constexpr auto operator()(Tuples && ... tuples) {
		if constexpr (sizeof...(tuples) == 0) {
			return tuple<>{};
		} else if constexpr (sizeof...(tuples) == 1) {
			return (..., OPERATORS_FORWARD(tuples));
		} else {
			return cat_impl(indexed_tuple{OPERATORS_FORWARD(tuples)}...);
		}
	}
};
export constexpr auto tuple_cat = tuple_cat_t();


struct apply_t {
private:
	template<std::size_t... indexes>
	static constexpr decltype(auto) implementation(auto && tuple_args, std::index_sequence<indexes...>, auto && function) {
		return OPERATORS_FORWARD(function)(OPERATORS_FORWARD(tuple_args)[bounded::constant<indexes>]...);
	}
public:
	static constexpr decltype(auto) operator()(auto && tuple_args, auto && function) {
		return implementation(
			OPERATORS_FORWARD(tuple_args),
			bounded::make_index_sequence(tuple_size<decltype(tuple_args)>),
			OPERATORS_FORWARD(function)
		);
	}
};
export constexpr auto apply = apply_t();


export template<std::size_t index>
constexpr auto && get(tuple_like auto && t) {
	return OPERATORS_FORWARD(t)[bounded::constant<index>];
}

}	// namespace tv

template<typename... Ts>
class std::tuple_size<tv::tuple<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<std::size_t index, typename... Ts>
class std::tuple_element<index, tv::tuple<Ts...>> : public tv::tuple_element_c<index, tv::tuple<Ts...>> {};
