// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <bounded/detail/class.hpp>
#include <bounded/detail/forward.hpp>

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

// TODO: properly support references?
template<typename GetFunction, typename... Ts>
struct basic_variant_base {
	template<typename F, typename Index, typename... Args> requires(
		std::is_convertible_v<F, GetFunction> and
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)
	constexpr basic_variant_base(std::in_place_t, F && function, Index index_, Args && ... args):
		m_function(BOUNDED_FORWARD(function)),
		m_data(get_index(index_, types<Ts>{}...), BOUNDED_FORWARD(args)...)
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
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)
	constexpr basic_variant_base(std::in_place_t, Index const index_, Args && ... args):
		basic_variant_base(
			std::in_place,
			GetFunction(get_index(index_, types<Ts>{}...)),
			index_,
			BOUNDED_FORWARD(args)...
		)
	{
	}
	
	template<matches_exactly_one_type<types<Ts>...> T>
	constexpr explicit basic_variant_base(T && value):
		basic_variant_base(
			std::in_place,
			types<std::decay_t<T>>{},
			BOUNDED_FORWARD(value)
		)
	{
	}
	
	template<matches_exactly_one_type<types<Ts>...> T> requires(
		std::is_constructible_v<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr void assignment(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = BOUNDED_FORWARD(value);
			} else {
				destroy(original);
				replace_active_member(types<std::decay_t<T>>{}, BOUNDED_FORWARD(value));
			}
		});
	}
	

	constexpr auto && get_function() const {
		return m_function;
	}
	constexpr auto && get_function() {
		return m_function;
	}


	constexpr auto index() const {
		return m_function(m_data);
	}

	template<typename Index, typename... Args>
	constexpr auto & emplace(Index index, Args && ... args) & requires requires { construct_return<typename decltype(get_type(index, types<Ts>{}...))::type>(BOUNDED_FORWARD(args)...); } {
		using indexed = typename decltype(get_type(index, types<Ts>{}...))::type;
		if constexpr (std::is_nothrow_constructible_v<indexed, Args...>) {
			visit(*this, destroy);
			return replace_active_member(index, BOUNDED_FORWARD(args)...);
		} else {
			auto & ref = operator[](index);
			auto value = construct_return<std::decay_t<decltype(ref)>>(BOUNDED_FORWARD(args)...);
			visit(*this, destroy);
			return replace_active_member(index, std::move(value));
		}
	}

	template<unique_type_identifier<types<Ts>...> Index>
	constexpr auto const & operator[](Index index_) const & {
		return operator_bracket(*this, index_);
	}
	template<unique_type_identifier<types<Ts>...> Index>
	constexpr auto & operator[](Index index_) & {
		return operator_bracket(*this, index_);
	}
	template<unique_type_identifier<types<Ts>...> Index>
	constexpr auto && operator[](Index index_) && {
		return operator_bracket(std::move(*this), index_);
	}

private:
	// Assumes the old object has already been destroyed
	template<typename Index, typename... Args>
	constexpr auto & replace_active_member(Index const index, Args && ... args) {
		constexpr auto trivial = (... and (
			std::is_trivially_copy_constructible_v<Ts> and
			std::is_trivially_copy_assignable_v<Ts> and
			std::is_trivially_destructible_v<Ts>
		));
		constexpr auto index_value = get_index(index, types<Ts>{}...);
		get_function() = GetFunction(index_value);
		if constexpr (trivial) {
			m_data = variadic_union_t<Ts...>(index_value, BOUNDED_FORWARD(args)...);
			return operator[](index_value);
		} else {
			return construct(operator[](index_value), BOUNDED_FORWARD(args)...);
		}
	}
	template<typename Variant, typename Index>
	static constexpr auto && operator_bracket(Variant && variant, Index const index_) {
		return ::bounded::detail::get_union_element(
			BOUNDED_FORWARD(variant).m_data,
			get_index(index_, types<Ts>{}...)
		);
	}
	
	[[no_unique_address]] GetFunction m_function;
	[[no_unique_address]] variadic_union_t<Ts...> m_data;
};


}	// namespace detail
}	// namespace bounded
