// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// std::pair does not implement noexcept specifications and does not currently
// work with std::is_constructible
//
// std::tuple does not implement noexcept specifications and does not support
// std::piecewise_construct

#pragma once

#include <containers/common_functions.hpp>
#include <containers/make_index_sequence.hpp>

#include <bounded/integer.hpp>

#include <type_traits>
#include <utility>

namespace containers {
using namespace bounded::literal;

namespace detail {

// Work around clang bug https://llvm.org/bugs/show_bug.cgi?id=26793
// When this is fixed, directly use a fold expression instead
template<typename... Args>
constexpr auto all(Args... args) noexcept {
	return (... and args);
}


constexpr struct not_piecewise_construct_t{} not_piecewise_construct{};

template<typename T>
constexpr auto is_derivable = std::is_class_v<T> and !std::is_final_v<T>;

// unique ensures that a tuple with the same type repeated shows the type as
// being different
// Ts... ensures that tuple<T, tuple<T>> does not create an ambiguous base
template<bool derivable, std::size_t unique, typename T, typename... Ts>
struct tuple_value;

template<std::size_t unique, typename T, typename... Ts>
using tuple_value_t = tuple_value<is_derivable<T>, unique, T, Ts...>;


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
struct tuple_impl<std::index_sequence<indexes...>, Types...> : tuple_value_t<indexes, Types, Types...>... {

	tuple_impl() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(
		::containers::detail::all(std::is_convertible_v<Args, Types>...) and
		::containers::detail::all(std::is_constructible<tuple_value_t<indexes, Types, Types...>, not_piecewise_construct_t, Args>::value...)
	)>
	constexpr tuple_impl(Args && ... args) noexcept((... and noexcept(tuple_value_t<indexes, Types, Types...>(not_piecewise_construct, std::forward<Args>(args))))):
		tuple_value_t<indexes, Types, Types...>(not_piecewise_construct, std::forward<Args>(args))...
	{
	}

	template<typename... Args, BOUNDED_REQUIRES(
		(... and std::is_constructible<tuple_value_t<indexes, Types, Types...>, std::piecewise_construct_t, Args>::value)
	)>
	constexpr tuple_impl(std::piecewise_construct_t, Args && ... args) noexcept(false):
		tuple_value_t<indexes, Types, Types...>(std::piecewise_construct, std::forward<Args>(args))...
	{
	}
};

template<typename... Types>
using tuple_impl_t = detail::tuple_impl<decltype(detail::make_index_sequence(bounded::constant<sizeof...(Types)>)), Types...>;

}	// namespace detail


template<typename... Types>
struct tuple : private detail::tuple_impl_t<Types...> {
	using detail::tuple_impl_t<Types...>::tuple_impl_t;

	// Work around https://bugs.llvm.org/show_bug.cgi?id=35655
	// This means we cannot have an auto template parameter here, so we have to
	// accept worse error messages when the user passes in something other than
	// a constant.
	template<typename Index, BOUNDED_REQUIRES(bounded::is_bounded_integer<Index> and Index{} < sizeof...(Types))>
	constexpr auto && operator[](Index const index_constant) const & noexcept {
		constexpr auto index_ = static_cast<std::size_t>(index_constant);
		return static_cast<detail::tuple_value_t<index_, detail::nth_type<index_, Types...>, Types...> const &>(*this).value();
	}
	template<typename Index, BOUNDED_REQUIRES(bounded::is_bounded_integer<Index> and Index{} < sizeof...(Types))>
	constexpr auto && operator[](Index const index_constant) & noexcept {
		constexpr auto index_ = static_cast<std::size_t>(index_constant);
		return static_cast<detail::tuple_value_t<index_, detail::nth_type<index_, Types...>, Types...> &>(*this).value();
	}
	template<typename Index, BOUNDED_REQUIRES(bounded::is_bounded_integer<Index> and Index{} < sizeof...(Types))>
	constexpr auto && operator[](Index const index_constant) && noexcept {
		constexpr auto index_ = static_cast<std::size_t>(index_constant);
		return static_cast<detail::tuple_value_t<index_, detail::nth_type<index_, Types...>, Types...> &&>(*this).value();
	}

protected:
	// Allow classes to explicitly expose that they are a tuple
	auto && as_tuple() const & noexcept {
		return *this;
	}
	auto && as_tuple() & noexcept {
		return *this;
	}
	auto && as_tuple() && noexcept {
		return std::move(*this);
	}
};

template<typename... Ts>
tuple(Ts && ...) -> tuple<std::decay_t<Ts>...>;


template<typename T>
constexpr auto is_tuple = false;

template<typename... Types>
constexpr auto is_tuple<tuple<Types...>> = true;



namespace detail {


template<std::size_t unique, typename T, typename... Ts>
struct tuple_value<true, unique, T, Ts...> : private T {
	tuple_value() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple_value(make_index_sequence(bounded::constant<sizeof...(Args)>), std::move(args))
	) {
	}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(not_piecewise_construct_t, Args && ... args) BOUNDED_NOEXCEPT_INITIALIZATION(
		T(std::forward<Args>(args)...)
	) {
	}
	
	constexpr auto && value() const & noexcept {
		return static_cast<T const &>(*this);
	}
	constexpr auto && value() & noexcept {
		return static_cast<T &>(*this);
	}
	constexpr auto && value() && noexcept {
		return static_cast<T &&>(*this);
	}

private:
	template<std::size_t... indexes, typename... Args>
	constexpr explicit tuple_value(std::index_sequence<indexes...>, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		T(args[bounded::constant<indexes>]...)
	) {
	}
};

// Work around https://bugs.llvm.org/show_bug.cgi?id=32385
template<std::size_t unique, typename T, typename... Ts>
struct tuple_value<true, unique, T const, Ts...> : private T {
	tuple_value() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple_value(make_index_sequence(bounded::constant<sizeof...(Args)>), std::move(args))
	) {
	}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(not_piecewise_construct_t, Args && ... args) BOUNDED_NOEXCEPT_INITIALIZATION(
		T(std::forward<Args>(args)...)
	) {
	}
	
	constexpr auto && value() const & noexcept {
		return static_cast<T const &>(*this);
	}

private:
	template<std::size_t... indexes, typename... Args>
	constexpr explicit tuple_value(std::index_sequence<indexes...>, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		T(args[bounded::constant<indexes>]...)
	) {
	}
};

template<std::size_t unique, typename T, typename... Ts>
struct tuple_value<false, unique, T, Ts...> {
	tuple_value() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple_value(make_index_sequence(bounded::constant<sizeof...(Args)>), std::move(args))
	) {
	}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args...>)>
	constexpr explicit tuple_value(not_piecewise_construct_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>):
		m_value(std::forward<Args>(args)...)
	{
	}

	constexpr auto && value() const & noexcept {
		return m_value;
	}
	constexpr auto && value() & noexcept {
		return m_value;
	}
	constexpr auto && value() && noexcept {
		// This should not be std::move because that would return an rvalue
		// reference even if T is an lvalue reference type.
		return static_cast<T &&>(m_value);
	}

private:
	template<std::size_t... indexes, typename... Args>
	constexpr explicit tuple_value(std::index_sequence<indexes...>, tuple<Args...> args) noexcept(std::is_nothrow_constructible_v<T, Args...>):
		m_value(args[bounded::constant<indexes>]...)
	{
	}

	T m_value;
};


template<std::size_t unique, typename... Ts>
struct tuple_value<false, unique, void, Ts...> {
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<>) noexcept {
	}
	constexpr explicit tuple_value(not_piecewise_construct_t) noexcept {
	}
};


// TODO: Proper noexcept
template<
	typename... lhs_types,
	typename... rhs_types,
	auto i
>
constexpr auto compare_impl(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs, bounded::constant_t<i> index) noexcept {
	if constexpr (index == sizeof...(lhs_types)) {
		return bounded::strong_ordering_equal;
	} else {
		using bounded::compare;
		if (auto const cmp = compare(lhs[index], rhs[index]); cmp != 0) {
			return cmp;
		}
		return compare_impl(lhs, rhs, index + 1_bi);
	}
}

template<
	typename... lhs_types,
	typename... rhs_types,
	BOUNDED_REQUIRES(sizeof...(lhs_types) == sizeof...(rhs_types))
>
constexpr auto compare(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	compare_impl(lhs, rhs, 0_bi)
)

}	// namespace detail



// TODO: unwrap reference_wrapper?
constexpr auto make_tuple = [](auto && ... args) BOUNDED_NOEXCEPT_VALUE(
	tuple<std::decay_t<decltype(args)>...>(std::forward<decltype(args)>(args)...)
);

constexpr auto tie = [](auto && ... args) noexcept {
		return tuple<decltype(args)...>(std::forward<decltype(args)>(args)...);
};

template<typename Tuple>
struct tuple_size_c;

template<typename... Types>
struct tuple_size_c<tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template<typename Tuple>
constexpr auto tuple_size = bounded::constant<tuple_size_c<std::decay_t<Tuple>>::value>;


template<std::size_t index, typename Tuple>
struct tuple_element_c;

template<std::size_t index, typename... Types>
struct tuple_element_c<index, tuple<Types...>> {
	using type = detail::nth_type<index, Types...>;
};

template<std::size_t index, typename Tuple>
using tuple_element = typename tuple_element_c<index, Tuple>::type;


constexpr struct tuple_cat_t {
private:
	template<std::size_t... first_indexes, typename First, std::size_t... second_indexes, typename Second>
	static constexpr auto cat_two(std::index_sequence<first_indexes...>, First && first, std::index_sequence<second_indexes...>, Second && second) BOUNDED_NOEXCEPT_VALUE(
		tuple<
			tuple_element<first_indexes, First>...,
			tuple_element<second_indexes, Second>...
		>(
			std::forward<First>(first)[bounded::constant<first_indexes>]...,
			std::forward<Second>(second)[bounded::constant<second_indexes>]...
		)
	)

	template<typename First, typename Second, typename... Tail>
	constexpr auto cat_many(First && first, Second && second, Tail && ... tail) const {
		return operator()(
			cat_two(
				::containers::detail::make_index_sequence(tuple_size<First>), std::forward<First>(first),
				::containers::detail::make_index_sequence(tuple_size<Second>), std::forward<Second>(second)
			),
			std::forward<Tail>(tail)...
		);
	}

public:
	constexpr auto operator()() const noexcept {
		return tuple<>();
	}

	template<typename Tuple>
	constexpr auto && operator()(Tuple && tuple) const noexcept {
		return std::forward<Tuple>(tuple);
	}

	template<typename... Tuples, BOUNDED_REQUIRES(
		(... and std::is_constructible<std::decay_t<Tuples>, Tuples &&>{})
	)>
	constexpr auto operator()(Tuples && ... tuples) const noexcept(
		(... and std::is_nothrow_constructible<std::decay_t<Tuples>, Tuples &&>{})
	) {
		return cat_many(std::forward<Tuples>(tuples)...);
	}
} tuple_cat;


constexpr struct apply_t {
private:
	template<typename Function, std::size_t... indexes, typename Tuple>
	static constexpr auto implementation(Function && f, std::index_sequence<indexes...>, Tuple && tuple_args) BOUNDED_NOEXCEPT_DECLTYPE(
		std::forward<Function>(f)(std::forward<Tuple>(tuple_args)[bounded::constant<indexes>]...)
	)
public:
	template<typename Function, typename Tuple>
	constexpr auto operator()(Function && f, Tuple && tuple_args) const BOUNDED_NOEXCEPT_DECLTYPE(
		implementation(
			std::forward<Function>(f),
			detail::make_index_sequence(tuple_size<Tuple>),
			std::forward<Tuple>(tuple_args)
		)
	)
} apply;


namespace detail {

template<typename Tuple, typename... Tuples, BOUNDED_REQUIRES((... and (tuple_size<Tuple> == tuple_size<Tuples>)))>
constexpr auto all_tuple_sizes() noexcept {
	return tuple_size<Tuple>;
}


#define CONTAINERS_FUNCTION_EXPRESSION std::declval<Function>()(std::declval<Tuples>()[bounded::constant<index>]...)
template<std::size_t index, typename Function, typename... Tuples>
constexpr auto is_noexcept_function_call = 
		(
			std::is_reference_v<decltype(CONTAINERS_FUNCTION_EXPRESSION)> or
			std::is_nothrow_move_constructible_v<decltype(CONTAINERS_FUNCTION_EXPRESSION)>
		) and
		noexcept(CONTAINERS_FUNCTION_EXPRESSION);
#undef CONTAINERS_FUNCTION_EXPRESSION

template<typename Function, typename... Tuples, std::size_t... indexes>
constexpr auto all_noexcept_function_calls_impl(std::index_sequence<indexes...>) noexcept {
	return (is_noexcept_function_call<indexes, Function, Tuples...> and ...);
}

template<typename Function, typename... Tuples>
constexpr auto all_noexcept_function_calls = all_noexcept_function_calls_impl<Function, Tuples...>(make_index_sequence(all_tuple_sizes<Tuples...>()));



template<auto i, typename Function, typename... Tuples>
constexpr auto transform_impl(bounded::constant_t<i> index, Function && function, Tuples && ... tuples) {
	if constexpr (i == all_tuple_sizes<Tuples...>()) {
		return tuple<>{};
	} else {
		return tuple_cat(
			tuple<decltype(function(std::forward<Tuples>(tuples)[index]...))>(function(std::forward<Tuples>(tuples)[index]...)),
			::containers::detail::transform_impl(index + 1_bi, function, std::forward<Tuples>(tuples)...)
		);
	}
}

}	// namespace detail

template<typename Function, typename... Tuples, BOUNDED_REQUIRES((... and is_tuple<std::decay_t<Tuples>>))>
constexpr auto transform(Function && function, Tuples && ... tuples) noexcept(detail::all_noexcept_function_calls<Function, Tuples && ...>) {
	return ::containers::detail::transform_impl(0_bi, function, std::forward<Tuples>(tuples)...);
}


}	// namespace containers
