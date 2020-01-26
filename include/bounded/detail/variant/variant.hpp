// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/copy_cv_ref.hpp>
#include <operators/forward.hpp>
#include <bounded/insert.hpp>
#include <bounded/lazy_init.hpp>
#include <bounded/is_constructible.hpp>
#include <bounded/value_to_function.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/special_member_functions.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <operators/returns.hpp>

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T>
concept copy_constructible = std::is_copy_constructible_v<T>;
template<typename T>
concept trivially_copy_constructible = copy_constructible<T> and std::is_trivially_copy_constructible_v<T>;

template<typename T>
concept move_constructible = std::is_move_constructible_v<T>;
template<typename T>
concept trivially_move_constructible = move_constructible<T> and std::is_trivially_move_constructible_v<T>;

template<typename T>
concept variant_copy_assignable = copy_constructible<T> and std::is_copy_assignable_v<T>;
template<typename T>
concept variant_trivially_copy_assignable = variant_copy_assignable<T> and trivially_copy_constructible<T> and std::is_trivially_copy_assignable_v<T>;

template<typename T>
concept variant_move_assignable = move_constructible<T> and std::is_move_assignable_v<T>;
template<typename T>
concept variant_trivially_move_assignable = variant_move_assignable<T> and trivially_move_constructible<T> and std::is_trivially_move_assignable_v<T>;

} // namespace detail

template<typename Function, typename... Ts>
concept unique_construct_function = matches_exactly_one_type<std::invoke_result_t<Function>, detail::types<Ts>...>;

template<typename GetFunction, typename... Ts>
struct basic_variant : private detail::variant_destructor<GetFunction, Ts...> {
private:
	template<typename Index>
	using type_at = typename decltype(detail::get_type(Index(), detail::types<Ts>()...))::type;

	template<typename Construct>
	using constructed_type = std::decay_t<std::invoke_result_t<Construct>>;
public:
	friend detail::variant_destructor<GetFunction, Ts...>;

	static_assert(std::is_trivially_copy_constructible_v<GetFunction>);
	static_assert(std::is_trivially_move_constructible_v<GetFunction>);
	static_assert(std::is_trivially_copy_assignable_v<GetFunction>);
	static_assert(std::is_trivially_move_assignable_v<GetFunction>);
	static_assert(std::is_trivially_destructible_v<GetFunction>);

	template<typename Index, typename Construct> requires(construct_function_for<Construct, type_at<Index>>)
	constexpr basic_variant(lazy_init_t, convertible_to<GetFunction> auto && function, Index index_, Construct && construct_):
		m_function(OPERATORS_FORWARD(function)),
		m_data(detail::get_index(index_, detail::types<Ts>()...), OPERATORS_FORWARD(construct_))
	{
		static_assert(
			min_value<decltype(index())> == 0_bi,
			"Type returned by your function must have a minimum value of 0."
		);
		static_assert(
			max_value<decltype(index())> == sizeof...(Ts) - 1,
			"Range of values of type returned by your function must equal the number of types."
		);
	}

	template<typename Index>
	constexpr basic_variant(convertible_to<GetFunction> auto && function, Index index_, convertible_to<type_at<Index>> auto && value):
		basic_variant(
			lazy_init,
			OPERATORS_FORWARD(function),
			index_,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}

	template<typename Index, typename Construct> requires construct_function_for<Construct, type_at<Index>>
	constexpr basic_variant(lazy_init_t, Index const index_, Construct && construct_):
		basic_variant(
			lazy_init,
			GetFunction(detail::get_index(index_, detail::types<Ts>()...)),
			index_,
			OPERATORS_FORWARD(construct_)
		)
	{
	}
	
	template<typename Index>
	constexpr basic_variant(Index index_, convertible_to<type_at<Index>> auto && value):
		basic_variant(
			lazy_init,
			index_,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}

	template<typename Construct> requires unique_construct_function<Construct, Ts...>
	constexpr explicit basic_variant(lazy_init_t, Construct && construct_):
		basic_variant(
			lazy_init,
			detail::types<constructed_type<Construct>>(),
			OPERATORS_FORWARD(construct_)
		)
	{
	}
	
	template<typename T> requires matches_exactly_one_type<T, detail::types<Ts>...>
	constexpr explicit basic_variant(T && value):
		basic_variant(
			lazy_init,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}
	

	constexpr basic_variant(basic_variant const &) requires (... and detail::trivially_copy_constructible<Ts>) = default;

	constexpr basic_variant(basic_variant const & other) noexcept(
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) requires((... and detail::copy_constructible<Ts>) and !(... and detail::trivially_copy_constructible<Ts>)):
		basic_variant(other, copy_move_tag{})
	{
	}


	constexpr basic_variant(basic_variant &&) requires(... and detail::trivially_move_constructible<Ts>) = default;

	constexpr basic_variant(basic_variant && other) noexcept(
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) requires((... and detail::move_constructible<Ts>) and !(... and detail::trivially_move_constructible<Ts>)):
		basic_variant(std::move(other), copy_move_tag{})
	{
	}

	constexpr auto operator=(basic_variant const &) & -> basic_variant & requires(... and detail::variant_trivially_copy_assignable<Ts>) = default;

	constexpr auto operator=(basic_variant const & other) & noexcept(
		(... and std::is_nothrow_copy_assignable_v<Ts>) and
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) -> basic_variant & requires((... and detail::variant_copy_assignable<Ts>) and !(... and detail::variant_trivially_copy_assignable<Ts>)) {
		assign(other);
		return *this;
	}


	constexpr auto operator=(basic_variant &&) & -> basic_variant & requires(... and detail::variant_trivially_move_assignable<Ts>) = default;

	constexpr auto operator=(basic_variant && other) & noexcept(
		(... and std::is_nothrow_move_assignable_v<Ts>) and
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) -> basic_variant & requires((... and detail::variant_move_assignable<Ts>) and !(... and detail::variant_trivially_move_assignable<Ts>)) {
		assign(std::move(other));
		return *this;
	}

	template<typename T> requires(
		!std::is_same_v<std::decay_t<T>, basic_variant> and
		constructible_from<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = OPERATORS_FORWARD(value);
			} else {
				destroy(original);
				replace_active_member(detail::types<std::decay_t<T>>(), value_to_function(OPERATORS_FORWARD(value)));
			}
		});
		return *this;
	}


	constexpr auto index() const {
		return m_function(m_data);
	}


	constexpr auto const & operator[](unique_type_identifier<detail::types<Ts>...> auto index_) const & {
		return operator_bracket(m_data, index_);
	}
	constexpr auto & operator[](unique_type_identifier<detail::types<Ts>...> auto index_) & {
		return operator_bracket(m_data, index_);
	}
	constexpr auto && operator[](unique_type_identifier<detail::types<Ts>...> auto index_) && {
		return operator_bracket(std::move(m_data), index_);
	}

	template<typename Index, construct_function_for<type_at<Index>> Construct>
	constexpr auto & emplace(Index index, Construct && construct_) & {
		if constexpr (noexcept(type_at<decltype(index)>(OPERATORS_FORWARD(construct_())))) {
			visit(*this, destroy);
			return replace_active_member(index, OPERATORS_FORWARD(construct_));
		} else {
			auto value = OPERATORS_FORWARD(construct_);
			visit(*this, destroy);
			return replace_active_member(index, value_to_function(std::move(value)));
		}
	}
	template<typename Construct> requires unique_construct_function<Construct, Ts...>
	constexpr auto & emplace(Construct && construct_) & {
		return emplace(detail::types<constructed_type<Construct>>(), OPERATORS_FORWARD(construct_));
	}
	
private:
	struct copy_move_tag{};
	constexpr basic_variant(auto && other, copy_move_tag):
		basic_variant(visit_with_index(
			OPERATORS_FORWARD(other),
			[&](auto parameter) {
				return basic_variant(
					other.m_function,
					parameter.index,
					[&] { return std::move(parameter).value; }
				);
			}
		))
	{
	}

	constexpr void assign(auto && other) {
		visit_with_index(
			*this,
			OPERATORS_FORWARD(other),
			[&](auto lhs, auto rhs) {
				if constexpr (lhs.index == rhs.index) {
					lhs.value = OPERATORS_FORWARD(rhs.value);
				} else {
					::bounded::insert(*this, rhs.index, OPERATORS_FORWARD(rhs.value));
				}
				m_function = other.m_function;
			}
		);
	}

	// Assumes the old object has already been destroyed
	constexpr auto & replace_active_member(auto const index, auto && construct_) {
		#if 0
		// TODO: This can be simplified when construct uses std::construct_at
		constexpr auto index_value = detail::get_index(index, detail::types<Ts>()...);
		m_function = GetFunction(index_value);
		return construct(operator[](index_value), OPERATORS_FORWARD(construct_));
		#endif
		constexpr auto trivial = (... and (
			std::is_trivially_copy_constructible_v<Ts> and
			std::is_trivially_copy_assignable_v<Ts> and
			std::is_trivially_destructible_v<Ts>
		));
		constexpr auto index_value = detail::get_index(index, detail::types<Ts>()...);
		m_function = GetFunction(index_value);
		if constexpr (trivial) {
			m_data = detail::variadic_union<Ts...>(index_value, OPERATORS_FORWARD(construct_));
			return operator[](index_value);
		} else {
			return construct(operator[](index_value), OPERATORS_FORWARD(construct_));
		}
	}

	static constexpr auto && operator_bracket(auto && data, auto const index_) {
		return ::bounded::detail::get_union_element(
			OPERATORS_FORWARD(data),
			detail::get_index(index_, detail::types<Ts>{}...)
		);
	}

	[[no_unique_address]] GetFunction m_function;
	[[no_unique_address]] detail::variadic_union<Ts...> m_data;
};

template<typename GetFunction, typename... Ts, typename T>
constexpr auto holds_alternative(basic_variant<GetFunction, Ts...> const & variant, detail::types<T> type) {
	return variant.index() == detail::get_index(type, detail::types<Ts>{}...);
}

namespace detail {

struct equality_visitor {
	template<typename T, auto n>
	constexpr auto operator()(visitor_parameter<T, n> const lhs, visitor_parameter<T, n> const rhs) const {
		return lhs.value == rhs.value;
	}
	template<typename LHS, auto lhs_n, typename RHS, auto rhs_n> requires(lhs_n != rhs_n)
	constexpr auto operator()(visitor_parameter<LHS, lhs_n>, visitor_parameter<RHS, rhs_n>) const {
		return false;
	}
};

} // namespace detail

template<typename GetFunction, equality_comparable... Ts>
constexpr auto operator==(basic_variant<GetFunction, Ts...> const & lhs, basic_variant<GetFunction, Ts...> const & rhs) {
	return visit_with_index(lhs, rhs, detail::equality_visitor{});
}


namespace detail {

template<std::size_t size>
struct variant_selector {
	template<auto n>
	constexpr explicit variant_selector(constant_t<n> index_):
		index(index_)
	{
	}
	constexpr auto operator()(auto const &) const {
		return index;
	}
private:
	integer<0, normalize<size - 1U>> index;
};

template<>
struct variant_selector<0> {
	variant_selector() = delete;
};

}	// namespace detail

template<typename... Ts>
using variant = basic_variant<detail::variant_selector<sizeof...(Ts)>, Ts...>;


}	// namespace bounded
