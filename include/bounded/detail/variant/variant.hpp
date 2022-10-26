// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/copy_cv_ref.hpp>
#include <bounded/concepts.hpp>
#include <operators/forward.hpp>
#include <bounded/insert.hpp>
#include <bounded/lazy_init.hpp>
#include <bounded/relocate.hpp>
#include <bounded/single_element_storage.hpp>
#include <bounded/value_to_function.hpp>
#include <bounded/detail/construct_destroy.hpp>
#include <bounded/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/variant_index.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <operators/returns.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T>
concept variant_copy_assignable = std::is_copy_constructible_v<T> and std::is_copy_assignable_v<T>;

template<typename T>
concept variant_trivially_copy_assignable = std::is_trivially_copy_constructible_v<T> and std::is_trivially_copy_assignable_v<T>;


template<typename T>
concept variant_move_assignable = std::is_move_constructible_v<T> and std::is_move_assignable_v<T>;

template<typename T>
concept variant_trivially_move_assignable = std::is_trivially_move_constructible_v<T> and std::is_trivially_move_assignable_v<T>;

inline constexpr auto equality_visitor = []
	<typename LHS, auto lhs_n, typename RHS, auto rhs_n>
	(visitor_parameter<LHS, lhs_n> const lhs, visitor_parameter<RHS, rhs_n> const rhs)
{
	if constexpr (lhs_n == rhs_n) {
		return lhs.value() == rhs.value();
	} else {
		return false;
	}
};

} // namespace detail

template<typename Function, typename... Ts>
concept unique_construct_function = matches_exactly_one_type<std::invoke_result_t<Function>, Ts...>;

template<typename... Ts>
struct variant {
private:
	template<typename Index>
	using type_at = typename decltype(detail::get_type(Index(), type<Ts>...))::type;

	template<typename Construct>
	using constructed_type = std::decay_t<std::invoke_result_t<Construct>>;
public:
	constexpr variant(
		lazy_init_t,
		auto index_,
		construct_function_for<type_at<decltype(index_)>> auto && construct_
	):
		m_index(detail::get_index(index_, type<Ts>...)),
		m_data(detail::get_index(index_, type<Ts>...), OPERATORS_FORWARD(construct_))
	{
	}

	constexpr variant(auto index_, std::convertible_to<type_at<decltype(index_)>> auto && value):
		variant(
			lazy_init,
			index_,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}

	constexpr variant(lazy_init_t, unique_construct_function<Ts...> auto && construct_):
		variant(
			lazy_init,
			type_t<constructed_type<decltype(construct_)>>(),
			OPERATORS_FORWARD(construct_)
		)
	{
	}
	
	constexpr explicit variant(matches_exactly_one_type<Ts...> auto && value):
		variant(
			lazy_init,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}
	

	constexpr variant(variant const &) = default;

	constexpr variant(variant const & other) noexcept(
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) requires((... and std::is_copy_constructible_v<Ts>) and !(... and std::is_trivially_copy_constructible_v<Ts>)):
		variant(other, copy_move_tag{})
	{
	}


	constexpr variant(variant &&) = default;

	constexpr variant(variant && other) noexcept(
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) requires((... and std::is_move_constructible_v<Ts>) and !(... and std::is_trivially_move_constructible_v<Ts>)):
		variant(std::move(other), copy_move_tag{})
	{
	}

	constexpr auto operator=(variant const &) & -> variant & = default;

	constexpr auto operator=(variant const & other) & noexcept(
		(... and std::is_nothrow_copy_assignable_v<Ts>) and
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) -> variant & requires((... and detail::variant_copy_assignable<Ts>) and !(... and detail::variant_trivially_copy_assignable<Ts>)) {
		assign(other);
		return *this;
	}


	constexpr auto operator=(variant &&) & -> variant & = default;

	constexpr auto operator=(variant && other) & noexcept(
		(... and std::is_nothrow_move_assignable_v<Ts>) and
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) -> variant & requires((... and detail::variant_move_assignable<Ts>) and !(... and detail::variant_trivially_move_assignable<Ts>)) {
		assign(std::move(other));
		return *this;
	}

	template<typename T> requires(
		(bounded::constant<0> + ... + bounded::constant<std::same_as<std::decay_t<T>, Ts>>) == bounded::constant<1> and
		constructible_from<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::same_as<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = OPERATORS_FORWARD(value);
			} else {
				this->emplace_impl(
					type_t<std::decay_t<T>>(),
					value_to_function(OPERATORS_FORWARD(value)),
					[&] { destroy(original); }
				);
			}
		});
		return *this;
	}

	~variant() = default;
	constexpr ~variant() requires(... or !std::is_trivially_destructible_v<Ts>) {
		visit(*this, destroy);
	}

	constexpr auto index() const -> variant_index<Ts...> {
		return m_index;
	}


	constexpr auto const & operator[](unique_type_identifier<Ts...> auto index_) const & {
		return operator_bracket(m_data, index_);
	}
	constexpr auto & operator[](unique_type_identifier<Ts...> auto index_) & {
		return operator_bracket(m_data, index_);
	}
	constexpr auto && operator[](unique_type_identifier<Ts...> auto index_) && {
		return operator_bracket(std::move(m_data), index_);
	}

	constexpr auto & emplace(auto index, construct_function_for<type_at<decltype(index)>> auto && construct_) & {
		return emplace_impl(index, OPERATORS_FORWARD(construct_), [&] { visit(*this, destroy); });
	}
	constexpr auto & emplace(unique_construct_function<Ts...> auto && construct_) & {
		return emplace(type_t<constructed_type<decltype(construct_)>>(), OPERATORS_FORWARD(construct_));
	}
	
	friend constexpr auto operator==(variant const & lhs, variant const & rhs) -> bool
		requires(... and equality_comparable<Ts>)
	{
		return visit_with_index(lhs, rhs, detail::equality_visitor);
	}

private:
	constexpr auto & emplace_impl(auto index, construct_function_for<type_at<decltype(index)>> auto && construct_, auto && destroy_active) & {
		using value_t = type_at<decltype(index)>;
		if constexpr (noexcept(value_t(OPERATORS_FORWARD(construct_)()))) {
			destroy_active();
			return replace_active_member(index, OPERATORS_FORWARD(construct_));
		} else {
			// TODO: Add fallback before this one that does trivial default
			// construction on exception
			auto storage = single_element_storage<value_t>{value_t(OPERATORS_FORWARD(construct_)())};
			destroy_active();
			return replace_active_member(index, [&]{ return bounded::relocate(storage.value); });
		}
	}

	// Assumes the old object has already been destroyed
	constexpr auto & replace_active_member(auto const index, auto && construct_) {
		constexpr auto index_value = detail::get_index(index, type<Ts>...);
		m_index = variant_index<Ts...>(index_value);
		return construct_at(m_data, [&] { return detail::variadic_union<Ts...>(index_value, OPERATORS_FORWARD(construct_)); });
	}

	struct copy_move_tag{};
	constexpr variant(auto && other, copy_move_tag):
		m_index(other.m_index),
		m_data(detail::uninitialized_union())
	{
		visit_with_index(
			OPERATORS_FORWARD(other),
			[&](auto parameter) {
				replace_active_member(parameter.index, [&] { return std::move(parameter).value(); });
			}
		);
	}

	constexpr void assign(auto && other) {
		visit_with_index(
			*this,
			OPERATORS_FORWARD(other),
			[&](auto lhs, auto rhs) {
				if constexpr (lhs.index == rhs.index) {
					lhs.value() = OPERATORS_FORWARD(rhs.value());
				} else {
					::bounded::insert(*this, rhs.index, OPERATORS_FORWARD(rhs.value()));
					m_index = variant_index<Ts...>(rhs.index);
				}
			}
		);
	}

	static constexpr auto && operator_bracket(auto && data, auto const index_) {
		return ::bounded::detail::get_union_element(
			OPERATORS_FORWARD(data),
			detail::get_index(index_, type<Ts>...)
		);
	}

	[[no_unique_address]] variant_index<Ts...> m_index;
	[[no_unique_address]] detail::variadic_union<Ts...> m_data;
};

}	// namespace bounded
