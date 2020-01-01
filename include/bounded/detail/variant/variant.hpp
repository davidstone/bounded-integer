// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/copy_cv_ref.hpp>
#include <bounded/forward.hpp>
#include <bounded/is_constructible.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/special_member_functions.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <type_traits>
#include <utility>

namespace bounded {

template<typename GetFunction, typename... Ts>
struct basic_variant : private detail::variant_destructor<GetFunction, Ts...> {
private:
	static inline constexpr bool has_trivial_copy_assignment =
		(... and std::is_trivially_copy_assignable_v<Ts>) and
		(... and std::is_trivially_copy_constructible_v<Ts>) and
		(... and std::is_trivially_destructible_v<Ts>);

	static inline constexpr bool has_trivial_move_assignment =
		(... and std::is_trivially_move_assignable_v<Ts>) and
		(... and std::is_trivially_move_constructible_v<Ts>) and
		(... and std::is_trivially_destructible_v<Ts>);

public:
	friend detail::variant_destructor<GetFunction, Ts...>;

	static_assert(std::is_trivially_copy_constructible_v<GetFunction>);
	static_assert(std::is_trivially_move_constructible_v<GetFunction>);
	static_assert(std::is_trivially_copy_assignable_v<GetFunction>);
	static_assert(std::is_trivially_move_assignable_v<GetFunction>);
	static_assert(std::is_trivially_destructible_v<GetFunction>);

	template<typename F, typename Index, typename... Args> requires(
		std::is_convertible_v<F, GetFunction> and
		is_constructible<typename decltype(detail::get_type(Index{}, detail::types<Ts>{}...))::type, Args...>
	)
	constexpr basic_variant(std::in_place_t, F && function, Index index_, Args && ... args):
		m_function(BOUNDED_FORWARD(function)),
		m_data(detail::get_index(index_, detail::types<Ts>{}...), BOUNDED_FORWARD(args)...)
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

	template<typename Index, typename... Args> requires(
		not std::is_convertible_v<Index, GetFunction> and
		is_constructible<typename decltype(detail::get_type(Index{}, detail::types<Ts>{}...))::type, Args...>
	)
	constexpr basic_variant(std::in_place_t, Index const index_, Args && ... args):
		basic_variant(
			std::in_place,
			GetFunction(detail::get_index(index_, detail::types<Ts>{}...)),
			index_,
			BOUNDED_FORWARD(args)...
		)
	{
	}
	
	template<matches_exactly_one_type<detail::types<Ts>...> T>
	constexpr explicit basic_variant(T && value):
		basic_variant(
			std::in_place,
			detail::types<std::decay_t<T>>{},
			BOUNDED_FORWARD(value)
		)
	{
	}
	

	constexpr basic_variant(basic_variant const &) requires(... and std::is_trivially_copy_constructible_v<Ts>) = default;

	constexpr basic_variant(basic_variant const & other) noexcept(
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) requires(
		(... and std::is_copy_constructible_v<Ts>) and
		!(... and std::is_trivially_copy_constructible_v<Ts>)
	):
		basic_variant(other, copy_move_tag{})
	{
	}


	constexpr basic_variant(basic_variant &&) requires(... and std::is_trivially_move_constructible_v<Ts>) = default;

	constexpr basic_variant(basic_variant && other) noexcept(
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) requires(
		(... and std::is_move_constructible_v<Ts>) and
		!(... and std::is_trivially_move_constructible_v<Ts>)
	):
		basic_variant(std::move(other), copy_move_tag{})
	{
	}


	constexpr auto operator=(basic_variant const &) & -> basic_variant & requires has_trivial_copy_assignment = default;

	constexpr auto operator=(basic_variant const & other) & noexcept(
		(... and std::is_nothrow_copy_assignable_v<Ts>) and
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) -> basic_variant & requires(
		(... and std::is_copy_assignable_v<Ts>) and
		(... and std::is_copy_constructible_v<Ts>) and
		!has_trivial_copy_assignment
	) {
		assign(other);
		return *this;
	}


	constexpr auto operator=(basic_variant &&) & -> basic_variant & requires has_trivial_move_assignment = default;

	constexpr auto operator=(basic_variant && other) & noexcept(
		(... and std::is_nothrow_move_assignable_v<Ts>) and
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) -> basic_variant & requires(
		(... and std::is_move_assignable_v<Ts>) and
		(... and std::is_move_constructible_v<Ts>) and
		!has_trivial_move_assignment
	) {
		assign(std::move(other));
		return *this;
	}

	template<typename T> requires(
		!std::is_same_v<std::decay_t<T>, basic_variant> and
		is_constructible<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = BOUNDED_FORWARD(value);
			} else {
				destroy(original);
				replace_active_member(detail::types<std::decay_t<T>>{}, BOUNDED_FORWARD(value));
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


	constexpr auto & emplace(auto index, auto && ... args) & requires requires { construct_return<typename decltype(detail::get_type(index, detail::types<Ts>{}...))::type>(BOUNDED_FORWARD(args)...); } {
		using indexed = typename decltype(detail::get_type(index, detail::types<Ts>{}...))::type;
		if constexpr (std::is_nothrow_constructible_v<indexed, decltype(args)...>) {
			visit(*this, destroy);
			return replace_active_member(index, BOUNDED_FORWARD(args)...);
		} else {
			auto & ref = operator[](index);
			auto value = construct_return<std::decay_t<decltype(ref)>>(BOUNDED_FORWARD(args)...);
			visit(*this, destroy);
			return replace_active_member(index, std::move(value));
		}
	}

private:
	struct copy_move_tag{};
	constexpr basic_variant(auto && other, copy_move_tag):
		basic_variant(visit_with_index(
			BOUNDED_FORWARD(other),
			[&](auto parameter) {
				return basic_variant(
					std::in_place,
					other.m_function,
					parameter.index,
					std::move(parameter).value
				);
			}
		))
	{
	}

	constexpr void assign(auto && other) {
		visit_with_index(
			*this,
			BOUNDED_FORWARD(other),
			[&](auto lhs, auto rhs) {
				if constexpr (lhs.index == rhs.index) {
					lhs.value = BOUNDED_FORWARD(rhs.value);
				} else {
					emplace(rhs.index, BOUNDED_FORWARD(rhs.value));
				}
				m_function = other.m_function;
			}
		);
	}

	// Assumes the old object has already been destroyed
	constexpr auto & replace_active_member(auto const index, auto && ... args) {
		constexpr auto trivial = (... and (
			std::is_trivially_copy_constructible_v<Ts> and
			std::is_trivially_copy_assignable_v<Ts> and
			std::is_trivially_destructible_v<Ts>
		));
		constexpr auto index_value = detail::get_index(index, detail::types<Ts>{}...);
		m_function = GetFunction(index_value);
		if constexpr (trivial) {
			m_data = detail::variadic_union<Ts...>(index_value, BOUNDED_FORWARD(args)...);
			return operator[](index_value);
		} else {
			return construct(operator[](index_value), BOUNDED_FORWARD(args)...);
		}
	}

	static constexpr auto && operator_bracket(auto && data, auto const index_) {
		return ::bounded::detail::get_union_element(
			BOUNDED_FORWARD(data),
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
