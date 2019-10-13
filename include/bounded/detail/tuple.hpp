// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/make_index_sequence.hpp>
#include <bounded/detail/requires.hpp>
#include <bounded/detail/type.hpp>

#include <type_traits>
#include <utility>

namespace bounded {

struct lazy_init_t {
} inline constexpr lazy_init;

namespace detail {

// TODO: File a bug against the standard?
template<typename T>
T declval();

template<typename T, typename... Args>
concept is_constructible = requires { T(declval<Args>()...); };

template<typename Function, typename T>
concept lazy_construct_function_for = (std::is_void_v<T> and std::is_void_v<decltype(std::declval<Function>()())>) or is_constructible<T, decltype(std::declval<Function>()())>;

// index is the index of this value within the tuple that contains it
template<std::size_t index, typename T>
struct tuple_value {
	tuple_value() = default;

	template<lazy_construct_function_for<T> Function>
	constexpr explicit tuple_value(lazy_init_t, Function function):
		m_value(std::move(function)())
	{
	}

	// TODO: convertible
	template<typename Arg> requires detail::is_constructible<T, Arg>
	constexpr explicit tuple_value(Arg && arg):
		m_value(BOUNDED_FORWARD(arg))
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

	constexpr auto && operator[](types<T>) const & {
		return m_value;
	}
	constexpr auto && operator[](types<T>) & {
		return m_value;
	}
	constexpr auto && operator[](types<T>) && {
		// This should not be std::move because that would return an rvalue
		// reference even if T is an lvalue reference type.
		return static_cast<T &&>(m_value);
	}

private:
	[[no_unique_address]] T m_value;
};

template<std::size_t index>
struct tuple_value<index, void> {
	tuple_value() = default;

	template<lazy_construct_function_for<void> Function>
	constexpr explicit tuple_value(lazy_init_t, Function function) {
		std::move(function)();
	}

	constexpr void operator[](constant_t<normalize<index>>) const {
	}
	constexpr void operator[](types<void>) const {
	}
};

template<std::size_t index, typename T, typename... Ts>
struct nth_type_c {
	using type = typename nth_type_c<index - 1, Ts...>::type;
};
template<typename T, typename... Ts>
struct nth_type_c<0, T, Ts...> {
	using type = T;
};
template<std::size_t index, typename... Ts>
using nth_type = typename nth_type_c<index, Ts...>::type;


template<typename Indexes, typename... Types>
struct tuple_impl;

template<std::size_t... indexes, typename... Types>
struct tuple_impl<std::index_sequence<indexes...>, Types...> : tuple_value<indexes, Types>... {

	tuple_impl() = default;

	template<typename... Args, BOUNDED_REQUIRES((... and std::is_convertible_v<Args, Types>))>
	constexpr tuple_impl(Args && ... args):
		tuple_value<indexes, Types>(BOUNDED_FORWARD(args))...
	{
	}

	template<typename... Functions> requires(... and lazy_construct_function_for<Functions, Types>)
	constexpr tuple_impl(lazy_init_t, Functions... functions):
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


namespace detail {

template<
	typename... lhs_types,
	typename... rhs_types,
	auto i
>
constexpr auto compare_impl(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs, constant_t<i> index) {
	if constexpr (index == constant<sizeof...(lhs_types)>) {
		return std::strong_ordering::equal;
	} else {
		if (auto const cmp = compare(lhs[index], rhs[index]); cmp != 0) {
			return cmp;
		}
		return compare_impl(lhs, rhs, index + constant<1>);
	}
}



template<typename LHS, typename RHS, std::size_t... indexes>
constexpr auto equal_impl(LHS const & lhs, RHS const & rhs, std::index_sequence<indexes...>) {
	return (... and (lhs[constant<indexes>] == rhs[constant<indexes>]));
}

}	// namespace detail

template<typename... lhs_types, typename... rhs_types> requires(sizeof...(lhs_types) == sizeof...(rhs_types))
constexpr auto compare(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs) {
	return detail::compare_impl(lhs, rhs, constant<0>);
}


template<typename... lhs_types, typename... rhs_types> requires(sizeof...(lhs_types) == sizeof...(rhs_types))
constexpr auto operator==(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs) {
	return detail::equal_impl(lhs, rhs, bounded::make_index_sequence(constant<sizeof...(lhs_types)>));
}



// TODO: unwrap reference_wrapper?
constexpr auto make_tuple = [](auto && ... args) {
	return tuple<std::decay_t<decltype(args)>...>(BOUNDED_FORWARD(args)...);
};

constexpr auto tie = [](auto && ... args) {
	return tuple<decltype(args)...>(BOUNDED_FORWARD(args)...);
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
	using type = detail::nth_type<index, Types...>;
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

inline constexpr struct tuple_cat_t {
private:
	template<std::size_t... first_indexes, typename First, std::size_t... second_indexes, typename Second, typename... Tail>
	constexpr auto cat_impl(
		indexed_tuple<std::index_sequence<first_indexes...>, First> first,
		indexed_tuple<std::index_sequence<second_indexes...>, Second> second,
		Tail && ... tail
	) const {
		return operator()(
			tuple<
				tuple_element<first_indexes, First>...,
				tuple_element<second_indexes, Second>...
			>(
				BOUNDED_FORWARD(first).tuple[constant<first_indexes>]...,
				BOUNDED_FORWARD(second).tuple[constant<second_indexes>]...
			),
			BOUNDED_FORWARD(tail).tuple...
		);
	}

public:
	template<typename... Tuples> requires(... and detail::is_constructible<std::decay_t<Tuples>, Tuples &&>)
	constexpr auto operator()(Tuples && ... tuples) const {
		if constexpr (sizeof...(Tuples) == 0) {
			return tuple<>{};
		} else if constexpr (sizeof...(Tuples) == 1) {
			return (..., BOUNDED_FORWARD(tuples));
		} else {
			return cat_impl(indexed_tuple{BOUNDED_FORWARD(tuples)}...);
		}
	}
} tuple_cat;


inline constexpr struct apply_t {
private:
	template<typename Tuple, std::size_t... indexes, typename Function>
	static constexpr decltype(auto) implementation(Tuple && tuple_args, std::index_sequence<indexes...>, Function && f) {
		return BOUNDED_FORWARD(f)(BOUNDED_FORWARD(tuple_args)[constant<indexes>]...);
	}
public:
	template<typename Tuple, typename Function>
	constexpr decltype(auto) operator()(Tuple && tuple_args, Function && f) const {
		return implementation(
			BOUNDED_FORWARD(tuple_args),
			bounded::make_index_sequence(tuple_size<Tuple>),
			BOUNDED_FORWARD(f)
		);
	}
} apply;


namespace detail {

template<typename Tuple, typename... Tuples> requires(... and (tuple_size<Tuple> == tuple_size<Tuples>))
constexpr auto all_tuple_sizes() {
	return tuple_size<Tuple>;
}


template<auto i, typename Function, typename... Tuples>
constexpr auto transform_impl(constant_t<i> index, Function && function, Tuples && ... tuples) {
	if constexpr (index == all_tuple_sizes<Tuples...>()) {
		return tuple<>{};
	} else {
		return tuple_cat(
			tuple<decltype(function(BOUNDED_FORWARD(tuples)[index]...))>(function(BOUNDED_FORWARD(tuples)[index]...)),
			detail::transform_impl(index + constant<1>, function, BOUNDED_FORWARD(tuples)...)
		);
	}
}

}	// namespace detail

template<typename Function, tuple_like... Tuples>
constexpr auto transform(Function && function, Tuples && ... tuples) {
	return detail::transform_impl(constant<0>, function, BOUNDED_FORWARD(tuples)...);
}


template<std::size_t index, typename... Ts>
constexpr auto && get(tuple<Ts...> const & t) {
	return t[constant<index>];
}
template<std::size_t index, typename... Ts>
constexpr auto && get(tuple<Ts...> & t) {
	return t[constant<index>];
}
template<std::size_t index, typename... Ts>
constexpr auto && get(tuple<Ts...> && t) {
	return std::move(t)[constant<index>];
}

}	// namespace bounded
namespace std {

template<typename... Ts>
class tuple_size<bounded::tuple<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<std::size_t index, typename... Ts>
class tuple_element<index, bounded::tuple<Ts...>> : public bounded::tuple_element_c<index, bounded::tuple<Ts...>> {};

} // namespace std