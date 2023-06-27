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


template<typename... Tuples>
constexpr auto all_tuple_sizes_equal = true;

template<typename Tuple, typename... Tuples>
constexpr auto all_tuple_sizes_equal<Tuple, Tuples...> = (... and (tuple_size<Tuple> == tuple_size<Tuples>));

template<typename... Tuples>
constexpr auto all_tuple_sizes = 0_bi;

template<typename Tuple, typename... Tuples>
constexpr auto all_tuple_sizes<Tuple, Tuples...> = tuple_size<Tuple>;

export template<tuple_like... Tuples>
constexpr auto transform(auto && function, Tuples && ... tuples) requires(all_tuple_sizes_equal<Tuples...>) {
	constexpr auto size = all_tuple_sizes<Tuples...>;
	auto single = [&](auto const index) -> decltype(auto) {
		return function(OPERATORS_FORWARD(tuples)[index]...);
	};
	auto all = [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
		return tuple<decltype(single(bounded::constant<indexes>))...>(single(bounded::constant<indexes>)...);
	};
	return all(std::make_index_sequence<static_cast<std::size_t>(size)>());
}


export template<std::size_t index>
constexpr auto && get(tuple_like auto && t) {
	return OPERATORS_FORWARD(t)[bounded::constant<index>];
}

}	// namespace tv

template<typename... Ts>
class std::tuple_size<tv::tuple<Ts...>> : public std::integral_constant<std::size_t, sizeof...(Ts)> {};

template<std::size_t index, typename... Ts>
class std::tuple_element<index, tv::tuple<Ts...>> : public tv::tuple_element_c<index, tv::tuple<Ts...>> {};

namespace {

static_assert(std::is_empty_v<tv::tuple<>>);
static_assert(tv::tuple() == tv::tuple());

struct empty {
	friend auto operator<=>(empty, empty) = default;
};

template<typename T, typename Index>
concept indexable_by = requires(T t, Index index) { t[index]; };

template<typename T, auto index>
concept constant_indexable_by = indexable_by<T, bounded::constant_t<index>>;

template<typename T, typename Index>
concept type_indexable_by = indexable_by<T, bounded::type_t<Index>>;


namespace tuple_empty {

using type = tv::tuple<empty>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::same_as<
	decltype(type()[bounded::type<empty>]),
	empty &&
>);

} // namespace tuple_empty

namespace tuple_empty_empty {

using type = tv::tuple<empty, empty>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	empty &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_empty_empty

namespace tuple_tuple {

using type = tv::tuple<tv::tuple<>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	tv::tuple<> &&
>);
static_assert(!constant_indexable_by<type, 1>);
static_assert(std::same_as<
	decltype(type()[bounded::type_t<tv::tuple<>>()]),
	tv::tuple<> &&
>);

} // namespace tuple_tuple

namespace tuple_empty_tuple_empty {

using type = tv::tuple<empty, tv::tuple<empty>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	tv::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(std::same_as<
	decltype(type()[bounded::type<empty>]),
	empty &&
>);
static_assert(std::same_as<
	decltype(type()[bounded::type_t<tv::tuple<empty>>()]),
	tv::tuple<empty> &&
>);

} // namespace tuple_empty_tuple_empty

namespace tuple_tuple_empty_tuple_empty {

using type = tv::tuple<tv::tuple<empty>, tv::tuple<empty>>;
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<type>);
#endif
static_assert(std::is_trivially_default_constructible_v<type>);
static_assert(std::is_trivially_copyable_v<type>);
static_assert(std::same_as<
	decltype(type()[0_bi]),
	tv::tuple<empty> &&
>);
static_assert(std::same_as<
	decltype(type()[1_bi]),
	tv::tuple<empty> &&
>);
static_assert(!constant_indexable_by<type, 2>);
static_assert(!type_indexable_by<type, tv::tuple<empty>>);
static_assert(!type_indexable_by<type, empty>);

} // namespace tuple_tuple_empty_tuple_empty

static_assert(std::is_empty_v<tv::tuple<void>>);

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<tv::tuple<void, void, empty, void>>);
#endif

static_assert(sizeof(tv::tuple<int>) == sizeof(int));
static_assert(sizeof(tv::tuple<int, tv::tuple<int>>) == sizeof(int) * 2);

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(sizeof(tv::tuple<int, empty>) == sizeof(int));
#endif

constexpr auto a = tv::tuple<int, empty, int>(0, empty{}, 5);

static_assert(a[0_bi] == 0);

static_assert(std::same_as<decltype(a[1_bi]), empty const &>);

static_assert(std::same_as<decltype(tv::tuple<int>{}[0_bi]), int &&>);
static_assert(std::same_as<decltype(tv::tuple<empty>{}[0_bi]), empty &&>);

struct different_empty {};
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98995
#if !defined __GNUC__ or defined __clang__
static_assert(std::is_empty_v<tv::tuple<empty, different_empty>>);
#endif
using nested_t = tv::tuple<tv::tuple<empty, different_empty>>;
constexpr auto nested = nested_t{};
static_assert(std::same_as<decltype(nested[0_bi][0_bi]), empty const &>);
static_assert(std::same_as<decltype(nested[0_bi][1_bi]), different_empty const &>);

static_assert(!indexable_by<nested_t, bounded::constant_t<1>>);

constexpr tv::tuple<int, char const *> b = { 2, "Hello" };
static_assert(b[0_bi] == b[0_bi]);

struct non_movable {
	non_movable() = default;
	non_movable(non_movable const &) = delete;
	non_movable(non_movable &&) = delete;

	friend auto operator<=>(non_movable const &, non_movable const &) = default;
};

static_assert(bounded::constructible_from<tv::tuple<non_movable>>);


constexpr auto non_movable_constructor = []{ return non_movable(); };
static_assert(non_movable_constructor() == non_movable());
static_assert(bounded::constructible_from<non_movable, non_movable>);
static_assert(tv::tuple_value<0, non_movable>(bounded::lazy_init, non_movable_constructor)[0_bi] == non_movable());
static_assert(bounded::constructible_from<tv::tuple<non_movable>, bounded::lazy_init_t, decltype(non_movable_constructor)>);
static_assert(std::same_as<tv::tuple_element<0, tv::tuple<non_movable>>, non_movable>);
constexpr tv::tuple<non_movable> non_movable_tuple = {};
static_assert(non_movable_tuple[0_bi] == non_movable{});

static_assert(a == a);
static_assert(b == b);
static_assert(non_movable_tuple == non_movable_tuple);
static_assert(a <= a);
static_assert(b <= b);
static_assert(non_movable_tuple <= non_movable_tuple);

constexpr auto d = tv::tuple(0, empty{}, 4);
static_assert(a != d);
static_assert(a > d);

struct {
	static constexpr auto operator()(float const x) {
		return static_cast<int>(x + 1);
	}
	static constexpr auto operator()(float const x, float const y) {
		return static_cast<int>(x + y);
	}
	static constexpr auto operator()(int const x) {
		return static_cast<float>(x + 1);
	}
	static constexpr auto operator()(int const x, int const y) {
		return static_cast<float>(x + y);
	}
} constexpr increment_and_swap_types{};

constexpr auto input_tuple = tv::make_tuple(0, 1.0F, 2, 3.0F, 4);
constexpr auto type_swapped = tv::transform(increment_and_swap_types, input_tuple);
static_assert(std::same_as<decltype(type_swapped), tv::tuple<float, int, float, int, float> const>);
static_assert(type_swapped == tv::make_tuple(1.0F, 2, 3.0F, 4, 5.0F));
constexpr auto merged = tv::transform(increment_and_swap_types, input_tuple, input_tuple);
static_assert(std::same_as<decltype(merged), tv::tuple<float, int, float, int, float> const>);
static_assert(merged == tv::make_tuple(0.0F, 2, 4.0F, 6, 8.0F));


}	// namespace
