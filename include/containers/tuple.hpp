// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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
// When this is fixed, use a fold expression instead
constexpr auto all() noexcept {
	return true;
}
template<typename... Args>
constexpr auto all(bool const arg, Args... args) noexcept {
	return arg and all(args...);
}


// std::pair does not implement noexcept specifications and does not currently
// work with std::is_constructible
//
// std::tuple does not implement noexcept specifications and does not support
// std::piecewise_construct

template<typename Indexes, typename... Types>
struct tuple_impl;


}	// namespace detail


template<typename... Types>
using tuple = detail::tuple_impl<detail::make_index_sequence<sizeof...(Types)>, Types...>;




namespace detail {

constexpr struct not_piecewise_construct_t{} not_piecewise_construct{};

template<typename T>
constexpr auto is_derivable = std::is_class<T>::value and !std::is_final<T>::value;

// unique ensures that a tuple with the same type repeated shows the type as
// being different
template<std::size_t unique, typename T, bool = is_derivable<T>>
struct tuple_value;



template<typename>
struct is_tuple_c : std::false_type {};

template<typename... Types>
struct is_tuple_c<tuple<Types...>> : std::true_type {};

}	// namespace detail

template<typename T>
constexpr auto is_tuple = detail::is_tuple_c<T>::value;



namespace detail {


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



template<std::size_t... indexes, typename... Types>
struct tuple_impl<std::index_sequence<indexes...>, Types...> : tuple_value<indexes, Types>... {

	tuple_impl() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(
		::containers::detail::all(std::is_convertible<Args, Types>::value...) and
		::containers::detail::all(std::is_constructible<tuple_value<indexes, Types>, not_piecewise_construct_t, Args>::value...)
	)>
	constexpr tuple_impl(Args && ... args) noexcept(false):
		tuple_value<indexes, Types>(not_piecewise_construct, std::forward<Args>(args))...
	{
	}

	template<typename... Args, BOUNDED_REQUIRES(
		::containers::detail::all(std::is_constructible<tuple_value<indexes, Types>, std::piecewise_construct_t, Args>::value...)
	)>
	constexpr tuple_impl(std::piecewise_construct_t, Args && ... args) noexcept(false):
		tuple_value<indexes, Types>(std::piecewise_construct, std::forward<Args>(args))...
	{
	}


	template<intmax_t index, BOUNDED_REQUIRES(index < sizeof...(Types))>
	constexpr auto && operator[](bounded::constant_t<index>) const & noexcept {
		return static_cast<tuple_value<index, nth_type<index, Types...>> const &>(*this).value();
	}
	template<intmax_t index, BOUNDED_REQUIRES(index < sizeof...(Types))>
	constexpr auto && operator[](bounded::constant_t<index>) & noexcept {
		return static_cast<tuple_value<index, nth_type<index, Types...>> &>(*this).value();
	}
	template<intmax_t index, BOUNDED_REQUIRES(index < sizeof...(Types))>
	constexpr auto && operator[](bounded::constant_t<index>) && noexcept {
		return static_cast<tuple_value<index, nth_type<index, Types...>> &&>(*this).value();
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


template<std::size_t unique, typename T>
struct tuple_value<unique, T, true> : private T {
	// We cannot just do using T::T here because that does not capture the
	// compiler-generated copy and move constructors.
	
	tuple_value() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<T, Args...>::value)>
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple_value(make_index_sequence<sizeof...(Args)>{}, std::move(args))
	) {
	}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<T, Args...>::value)>
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

template<std::size_t unique, typename T>
struct tuple_value<unique, T, false> {
	tuple_value() = default;
	
	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<T, Args...>::value)>
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<Args...> args) BOUNDED_NOEXCEPT_INITIALIZATION(
		tuple_value(make_index_sequence<sizeof...(Args)>{}, std::move(args))
	) {
	}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<T, Args...>::value)>
	constexpr explicit tuple_value(not_piecewise_construct_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args...>::value):
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
	constexpr explicit tuple_value(std::index_sequence<indexes...>, tuple<Args...> args) noexcept(std::is_nothrow_constructible<T, Args...>::value):
		m_value(args[bounded::constant<indexes>]...)
	{
	}

	T m_value;
};


template<std::size_t unique>
struct tuple_value<unique, void, false> {
	constexpr explicit tuple_value(std::piecewise_construct_t, tuple<>) noexcept {
	}
	constexpr explicit tuple_value(not_piecewise_construct_t) noexcept {
	}
};



template<std::size_t... lhs_indexes, typename... lhs_types, std::size_t... rhs_indexes, typename... rhs_types>
constexpr auto operator==(tuple_impl<std::index_sequence<lhs_indexes...>, lhs_types...> const & lhs, tuple_impl<std::index_sequence<rhs_indexes...>, rhs_types...> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	(... and (lhs[bounded::constant<lhs_indexes>] == rhs[bounded::constant<rhs_indexes>]))
)


template<
	typename... lhs_types,
	typename... rhs_types
>
constexpr auto less_impl(tuple<lhs_types...> const &, tuple<rhs_types...> const &, bounded::constant_t<sizeof...(lhs_types)>) noexcept {
	return std::false_type{};
}

template<
	typename... lhs_types,
	typename... rhs_types,
	std::intmax_t i
>
constexpr auto less_impl(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs, bounded::constant_t<i> index) BOUNDED_NOEXCEPT_VALUE(
	lhs[index] < rhs[index] or (!(rhs[index] < lhs[index]) and less_impl(lhs, rhs, index + 1_bi))
)

template<
	typename... lhs_types,
	typename... rhs_types,
	BOUNDED_REQUIRES(sizeof...(lhs_types) == sizeof...(rhs_types))
>
constexpr auto operator<(tuple<lhs_types...> const & lhs, tuple<rhs_types...> const & rhs) BOUNDED_NOEXCEPT_VALUE(
	less_impl(lhs, rhs, 0_bi)
)

}	// namespace detail



// TODO: unwrap reference_wrapper?
// TODO: Should this actually be decay?
template<typename... Types>
constexpr auto make_tuple(Types && ... args) BOUNDED_NOEXCEPT_VALUE(
	tuple<std::decay_t<Types>...>(std::forward<Types>(args)...)
)

template<typename... Types>
constexpr auto tie(Types & ... args) noexcept {
	return tuple<Types &...>(args...);
}

template<typename... Types>
constexpr auto forward_as_tuple(Types && ... args) noexcept {
	return tuple<Types && ...>(std::forward<Types>(args)...);
}


template<typename Tuple>
struct tuple_size_c;

template<typename... Types>
struct tuple_size_c<tuple<Types...>> : std::integral_constant<std::intmax_t, sizeof...(Types)> {};

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


namespace detail {

template<std::size_t... first_indexes, typename First, std::size_t... second_indexes, typename Second>
constexpr auto tuple_cat_impl(std::index_sequence<first_indexes...>, First && first, std::index_sequence<second_indexes...>, Second && second) BOUNDED_NOEXCEPT_VALUE(
	tuple<
		tuple_element<first_indexes, First>...,
		tuple_element<second_indexes, Second>...
	>(
		std::forward<First>(first)[bounded::constant<first_indexes>]...,
		std::forward<Second>(second)[bounded::constant<second_indexes>]...
	)
)

}	// namespace detail

constexpr auto tuple_cat() noexcept {
	return tuple<>();
}

template<typename Tuple>
constexpr auto tuple_cat(Tuple && tuple) BOUNDED_NOEXCEPT_VALUE(
	std::forward<Tuple>(tuple)
)

template<typename First, typename Second, typename... Tail>
constexpr auto tuple_cat(First && first, Second && second, Tail && ... tail) BOUNDED_NOEXCEPT_VALUE(
	tuple_cat(
		::containers::detail::tuple_cat_impl(
			::containers::detail::make_index_sequence<tuple_size<First>.value()>{}, std::forward<First>(first),
			::containers::detail::make_index_sequence<tuple_size<Second>.value()>{}, std::forward<Second>(second)
		), 
		std::forward<Tail>(tail)...
	)
)



namespace detail {

template<typename Function, std::size_t... indexes, typename Tuple>
constexpr auto apply_helper(Function && f, std::index_sequence<indexes...>, Tuple && tuple_args) BOUNDED_NOEXCEPT_DECLTYPE(
	std::forward<Function>(f)(std::forward<Tuple>(tuple_args)[bounded::constant<indexes>]...)
)

}	// namespace detail

template<typename Function, typename Tuple>
constexpr auto apply(Function && f, Tuple && tuple_args) BOUNDED_NOEXCEPT_DECLTYPE(
	::containers::detail::apply_helper(
		std::forward<Function>(f),
		detail::make_index_sequence<tuple_size<Tuple>.value()>{},
		std::forward<Tuple>(tuple_args)
	)
)



namespace detail {

struct sfinae_empty {};

template<typename... Tuples>
struct all_tuple_sizes;

template<typename Tuple, typename... Tuples>
struct all_tuple_sizes<Tuple, Tuples...> : std::conditional_t<
	all_tuple_sizes<Tuple>::value == all_tuple_sizes<Tuples...>::value,
	all_tuple_sizes<Tuple>,
	sfinae_empty
> {};

template<typename Tuple>
struct all_tuple_sizes<Tuple> : std::integral_constant<std::intmax_t, tuple_size<Tuple>.value()> {};


template<std::intmax_t i, typename Function, typename... Tuples, BOUNDED_REQUIRES(i == all_tuple_sizes<Tuples...>::value)>
constexpr auto transform_impl(bounded::constant_t<i>, Function &&, Tuples && ...) noexcept {
	return tuple<>{};
}
// TODO: noexcept specification
template<std::intmax_t i, typename Function, typename... Tuples, BOUNDED_REQUIRES(i != all_tuple_sizes<Tuples...>::value)>
constexpr auto transform_impl(bounded::constant_t<i> index, Function && function, Tuples && ... tuples) noexcept(false) {
	return ::containers::tuple_cat(
		tuple<decltype(function(std::forward<Tuples>(tuples)[index]...))>(function(std::forward<Tuples>(tuples)[index]...)),
		::containers::detail::transform_impl(index + 1_bi, function, std::forward<Tuples>(tuples)...)
	);
}

}	// namespace detail

template<typename Function, typename... Tuples>
constexpr auto transform(Function && function, Tuples && ... tuples) BOUNDED_NOEXCEPT_VALUE(
	::containers::detail::transform_impl(0_bi, std::forward<Function>(function), std::forward<Tuples>(tuples)...)
)


// TODO: uses_allocator?


}	// namespace containers
