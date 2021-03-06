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
#include <bounded/value_to_function.hpp>
#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <operators/returns.hpp>

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


struct non_constructible {
	non_constructible() = delete;
};

template<std::size_t size>
struct variant_selector_c {
	using type = integer<0, normalize<size - 1U>>;
};

template<>
struct variant_selector_c<0> {
	using type = non_constructible;
};

template<std::size_t size>
using variant_selector = typename variant_selector_c<size>::type;

} // namespace detail

template<typename Function, typename... Ts>
concept unique_construct_function = matches_exactly_one_type<std::invoke_result_t<Function>, Ts...>;

namespace detail {

template<typename... Ts>
struct variant_impl {
private:
	template<typename Index>
	using type_at = typename decltype(detail::get_type(Index(), detail::types<Ts>()...))::type;

	template<typename Construct>
	using constructed_type = std::decay_t<std::invoke_result_t<Construct>>;
public:
	constexpr variant_impl(
		lazy_init_t,
		auto index_,
		construct_function_for<type_at<decltype(index_)>> auto && construct_
	):
		m_index(detail::get_index(index_, detail::types<Ts>()...)),
		m_data(detail::get_index(index_, detail::types<Ts>()...), OPERATORS_FORWARD(construct_))
	{
	}

	constexpr variant_impl(auto index_, convertible_to<type_at<decltype(index_)>> auto && value):
		variant_impl(
			lazy_init,
			index_,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}

	constexpr variant_impl(lazy_init_t, unique_construct_function<Ts...> auto && construct_):
		variant_impl(
			lazy_init,
			detail::types<constructed_type<decltype(construct_)>>(),
			OPERATORS_FORWARD(construct_)
		)
	{
	}
	
	constexpr explicit variant_impl(matches_exactly_one_type<Ts...> auto && value):
		variant_impl(
			lazy_init,
			value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}
	

	constexpr variant_impl(variant_impl const &) = default;

	constexpr variant_impl(variant_impl const & other) noexcept(
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) requires((... and std::is_copy_constructible_v<Ts>) and !(... and std::is_trivially_copy_constructible_v<Ts>)):
		variant_impl(other, copy_move_tag{})
	{
	}


	constexpr variant_impl(variant_impl &&) = default;

	constexpr variant_impl(variant_impl && other) noexcept(
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) requires((... and std::is_move_constructible_v<Ts>) and !(... and std::is_trivially_move_constructible_v<Ts>)):
		variant_impl(std::move(other), copy_move_tag{})
	{
	}

	constexpr auto operator=(variant_impl const &) & -> variant_impl & = default;

	constexpr auto operator=(variant_impl const & other) & noexcept(
		(... and std::is_nothrow_copy_assignable_v<Ts>) and
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) -> variant_impl & requires((... and detail::variant_copy_assignable<Ts>) and !(... and detail::variant_trivially_copy_assignable<Ts>)) {
		assign(other);
		return *this;
	}


	constexpr auto operator=(variant_impl &&) & -> variant_impl & = default;

	constexpr auto operator=(variant_impl && other) & noexcept(
		(... and std::is_nothrow_move_assignable_v<Ts>) and
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) -> variant_impl & requires((... and detail::variant_move_assignable<Ts>) and !(... and detail::variant_trivially_move_assignable<Ts>)) {
		assign(std::move(other));
		return *this;
	}

	constexpr auto index() const {
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
		if constexpr (noexcept(type_at<decltype(index)>(OPERATORS_FORWARD(construct_())))) {
			visit(*this, destroy);
			return replace_active_member(index, OPERATORS_FORWARD(construct_));
		} else {
			auto value = OPERATORS_FORWARD(construct_)();
			visit(*this, destroy);
			return replace_active_member(index, value_to_function(std::move(value)));
		}
	}
	constexpr auto & emplace(unique_construct_function<Ts...> auto && construct_) & {
		return emplace(detail::types<constructed_type<decltype(construct_)>>(), OPERATORS_FORWARD(construct_));
	}
	
	// Assumes the old object has already been destroyed
	constexpr auto & replace_active_member(auto const index, auto && construct_) {
		constexpr auto index_value = detail::get_index(index, detail::types<Ts>()...);
		m_index = index_value;
		return construct(m_data, [&] { return detail::variadic_union<Ts...>(index_value, OPERATORS_FORWARD(construct_)); });
	}

private:
	struct copy_move_tag{};
	constexpr variant_impl(auto && other, copy_move_tag):
		m_index(other.m_index),
		m_data(uninitialized_union())
	{
		visit_with_index(
			OPERATORS_FORWARD(other),
			[&](auto parameter) {
				replace_active_member(parameter.index, [&] { return std::move(parameter).value; });
			}
		);
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
					m_index = rhs.index;
				}
			}
		);
	}

	static constexpr auto && operator_bracket(auto && data, auto const index_) {
		return ::bounded::detail::get_union_element(
			OPERATORS_FORWARD(data),
			detail::get_index(index_, detail::types<Ts>{}...)
		);
	}

	[[no_unique_address]] detail::variant_selector<sizeof...(Ts)> m_index;
	[[no_unique_address]] detail::variadic_union<Ts...> m_data;
};

} // namespace detail

template<typename... Ts>
struct variant : private detail::variant_impl<Ts...> {
	using detail::variant_impl<Ts...>::variant_impl;
	using detail::variant_impl<Ts...>::index;
	using detail::variant_impl<Ts...>::operator[];
	using detail::variant_impl<Ts...>::emplace;

	variant(variant const &) = default;
	variant(variant &&) = default;
	variant & operator=(variant const &) & = default;
	variant & operator=(variant &&) & = default;

	constexpr ~variant() {
		visit(*this, destroy);
	}

	template<typename T> requires(
		!std::is_same_v<std::decay_t<T>, variant> and
		constructible_from<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = OPERATORS_FORWARD(value);
			} else {
				destroy(original);
				this->replace_active_member(detail::types<std::decay_t<T>>(), value_to_function(OPERATORS_FORWARD(value)));
			}
		});
		return *this;
	}
};

template<typename... Ts> requires(... and std::is_trivially_destructible_v<Ts>)
struct variant<Ts...> : private detail::variant_impl<Ts...> {
	using detail::variant_impl<Ts...>::variant_impl;
	using detail::variant_impl<Ts...>::index;
	using detail::variant_impl<Ts...>::operator[];
	using detail::variant_impl<Ts...>::emplace;

	template<typename T> requires(
		!std::is_same_v<std::decay_t<T>, variant> and
		constructible_from<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = OPERATORS_FORWARD(value);
			} else {
				destroy(original);
				this->replace_active_member(detail::types<std::decay_t<T>>(), value_to_function(OPERATORS_FORWARD(value)));
			}
		});
		return *this;
	}
};

template<typename... Ts, typename T>
constexpr auto holds_alternative(variant<Ts...> const & variant, detail::types<T> type) {
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

template<equality_comparable... Ts>
constexpr auto operator==(variant<Ts...> const & lhs, variant<Ts...> const & rhs) -> bool {
	return visit_with_index(lhs, rhs, detail::equality_visitor{});
}

}	// namespace bounded
