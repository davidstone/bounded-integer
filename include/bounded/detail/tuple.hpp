// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <operators/forward.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/make_index_sequence.hpp>
#include <bounded/type.hpp>

#include <bounded/concepts.hpp>
#include <bounded/lazy_init.hpp>
#include <bounded/nth_type.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<std::size_t... indexes>
constexpr auto compare_impl(auto const & lhs, auto const & rhs, std::index_sequence<indexes...>) {
	auto cmp = std::strong_ordering::equal;
	void((... or ((cmp = lhs[constant<indexes>] <=> rhs[constant<indexes>]), cmp != 0)));
	return cmp;
}

template<std::size_t... indexes>
constexpr auto equal_impl(auto const & lhs, auto const & rhs, std::index_sequence<indexes...>) {
	return (... and (lhs[constant<indexes>] == rhs[constant<indexes>]));
}


// index is the index of this value within the tuple that contains it
template<std::size_t index, typename T>
struct tuple_value {
	tuple_value() = default;

	constexpr explicit tuple_value(lazy_init_t, construct_function_for<T> auto function):
		m_value(std::move(function)())
	{
	}

	constexpr explicit tuple_value(std::convertible_to<T> auto && arg):
		m_value(OPERATORS_FORWARD(arg))
	{
	}

	constexpr auto && operator[](constant_t<normalize<index>>) const & {
		return m_value;
	}
	constexpr auto && operator[](constant_t<normalize<index>>) & {
		return m_value;
	}
	constexpr auto && operator[](constant_t<normalize<index>>) && {
		// This should not be std::move because that would return an rvalue
		// reference even if T is an lvalue reference type.
		return static_cast<T &&>(m_value);
	}

	constexpr auto && operator[](type_t<T>) const & {
		return m_value;
	}
	constexpr auto && operator[](type_t<T>) & {
		return m_value;
	}
	constexpr auto && operator[](type_t<T>) && {
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

	constexpr explicit tuple_value(lazy_init_t, construct_function_for<void> auto function) {
		std::move(function)();
	}

	constexpr void operator[](constant_t<normalize<index>>) const {
	}
	constexpr void operator[](type_t<void>) const {
	}
};

template<typename Indexes, typename... Types>
struct tuple_impl;

template<std::size_t... indexes, typename... Types>
struct tuple_impl<std::index_sequence<indexes...>, Types...> : tuple_value<indexes, Types>... {

	tuple_impl() = default;

	constexpr tuple_impl(std::convertible_to<Types> auto && ... args):
		tuple_value<indexes, Types>(OPERATORS_FORWARD(args))...
	{
	}

	constexpr tuple_impl(lazy_init_t, construct_function_for<Types> auto ... functions):
		tuple_value<indexes, Types>(lazy_init, std::move(functions))...
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
using tuple_impl_t = detail::tuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...>;

}	// namespace detail


template<typename... Types>
struct tuple : private detail::tuple_impl_t<Types...> {
	using detail::tuple_impl_t<Types...>::tuple_impl_t;
	using detail::tuple_impl_t<Types...>::operator[];

	template<ordered<Types>... RHSTypes>
	friend constexpr auto operator<=>(tuple const & lhs, tuple<RHSTypes...> const & rhs) {
		return detail::compare_impl(lhs, rhs, bounded::make_index_sequence(constant<sizeof...(Types)>));
	}

	template<equality_comparable<Types>... RHSTypes>
	friend constexpr auto operator==(tuple const & lhs, tuple<RHSTypes...> const & rhs) -> bool {
		return detail::equal_impl(lhs, rhs, bounded::make_index_sequence(constant<sizeof...(Types)>));
	}
};

template<typename... Ts>
tuple(Ts && ...) -> tuple<std::decay_t<Ts>...>;


namespace detail {

template<typename T>
inline constexpr auto is_tuple = false;

template<typename... Types>
inline constexpr auto is_tuple<tuple<Types...>> = true;

} // namespace detail

// TODO: tuple_like to be based on requirements, not exact match
template<typename T>
concept tuple_like = detail::is_tuple<std::decay_t<T>>;


// TODO: unwrap reference_wrapper?
constexpr auto make_tuple = [](auto && ... args) {
	return tuple<std::decay_t<decltype(args)>...>(OPERATORS_FORWARD(args)...);
};

constexpr auto tie = [](auto && ... args) {
	return tuple<decltype(args)...>(OPERATORS_FORWARD(args)...);
};

template<typename Tuple>
struct tuple_size_c;

template<typename... Types>
struct tuple_size_c<tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template<typename Tuple>
inline constexpr auto tuple_size = constant<tuple_size_c<std::decay_t<Tuple>>::value>;


template<std::size_t index, typename Tuple>
struct tuple_element_c;

template<std::size_t index, typename... Types>
struct tuple_element_c<index, tuple<Types...>> {
	using type = nth_type<index, Types...>;
};

template<std::size_t index, typename Tuple>
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
				OPERATORS_FORWARD(first).tuple[constant<first_indexes>]...,
				OPERATORS_FORWARD(second).tuple[constant<second_indexes>]...
			),
			OPERATORS_FORWARD(tail).tuple...
		);
	}

public:
	template<typename... Tuples> requires(... and constructible_from<std::decay_t<Tuples>, Tuples &&>)
	static constexpr auto operator()(Tuples && ... tuples) {
		if constexpr (sizeof...(tuples) == 0) {
			return tuple<>{};
		} else if constexpr (sizeof...(tuples) == 1) {
			return (..., OPERATORS_FORWARD(tuples));
		} else {
			return cat_impl(indexed_tuple{OPERATORS_FORWARD(tuples)}...);
		}
	}
} constexpr inline tuple_cat;


struct apply_t {
private:
	template<std::size_t... indexes>
	static constexpr decltype(auto) implementation(auto && tuple_args, std::index_sequence<indexes...>, auto && function) {
		return OPERATORS_FORWARD(function)(OPERATORS_FORWARD(tuple_args)[constant<indexes>]...);
	}
public:
	static constexpr decltype(auto) operator()(auto && tuple_args, auto && function) {
		return implementation(
			OPERATORS_FORWARD(tuple_args),
			bounded::make_index_sequence(tuple_size<decltype(tuple_args)>),
			OPERATORS_FORWARD(function)
		);
	}
} constexpr inline apply;


namespace detail {

template<typename... Tuples>
inline constexpr auto all_tuple_sizes_equal = true;

template<typename Tuple, typename... Tuples>
inline constexpr auto all_tuple_sizes_equal<Tuple, Tuples...> = (... and (tuple_size<Tuple> == tuple_size<Tuples>));

template<typename... Tuples>
inline constexpr auto all_tuple_sizes = constant<0>;

template<typename Tuple, typename... Tuples>
inline constexpr auto all_tuple_sizes<Tuple, Tuples...> = tuple_size<Tuple>;

}	// namespace detail

template<tuple_like... Tuples>
constexpr auto transform(auto && function, Tuples && ... tuples) requires(detail::all_tuple_sizes_equal<Tuples...>) {
	constexpr auto size = detail::all_tuple_sizes<Tuples...>;
	auto single = [&](auto const index) -> decltype(auto) {
		return function(OPERATORS_FORWARD(tuples)[index]...);
	};
	auto all = [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
		return tuple<decltype(single(constant<indexes>))...>(single(constant<indexes>)...);
	};
	return all(std::make_index_sequence<static_cast<std::size_t>(size)>());
}


template<std::size_t index>
constexpr auto && get(tuple_like auto && t) {
	return OPERATORS_FORWARD(t)[constant<index>];
}

}	// namespace bounded

template<typename... Ts>
class std::tuple_size<bounded::tuple<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<std::size_t index, typename... Ts>
class std::tuple_element<index, bounded::tuple<Ts...>> : public bounded::tuple_element_c<index, bounded::tuple<Ts...>> {};
