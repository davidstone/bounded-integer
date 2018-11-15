// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>
#include <bounded/detail/variant/get_index.hpp>
#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/variant/variadic_union.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <bounded/detail/class.hpp>
#include <bounded/detail/forward.hpp>

#include <utility>

namespace bounded {
namespace detail {

// TODO: properly support references?
// TODO: exception safety
template<typename GetFunction, typename... Ts>
struct basic_variant_base {
	// TODO: assert GetFunction is noexcept

	template<typename F, typename Index, typename... Args, BOUNDED_REQUIRES(
		std::is_convertible_v<F, GetFunction> and
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)>
	constexpr basic_variant_base(std::in_place_t, F && function, Index index_, Args && ... args) noexcept(
		std::is_nothrow_constructible_v<GetFunction, F> and
		std::is_nothrow_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	):
		m_data(
			std::piecewise_construct,
			tie(BOUNDED_FORWARD(function)),
			tie(get_index(index_, types<Ts>{}...), BOUNDED_FORWARD(args)...)
		)
	{
		static_assert(
			decltype(index())::min() == 0_bi,
			"Type returned by your function must have a minimum value of 0."
		);
		static_assert(
			decltype(index())::max() == sizeof...(Ts) - 1,
			"Range of values of type returned by your function must equal the number of types."
		);
	}
	
	template<typename Index, typename... Args, BOUNDED_REQUIRES(
		not std::is_convertible_v<Index, GetFunction> and
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)>
	constexpr basic_variant_base(std::in_place_t, Index const index_, Args && ... args) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_variant_base(
			std::in_place,
			GetFunction(get_index(index_, types<Ts>{}...)),
			index_,
			BOUNDED_FORWARD(args)...
		)
	) {
	}
	
	template<typename T, BOUNDED_REQUIRES(is_valid_index(types<std::decay_t<T>>{}, types<Ts>{}...))>
	constexpr explicit basic_variant_base(T && value) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_variant_base(
			std::in_place,
			types<std::decay_t<T>>{},
			BOUNDED_FORWARD(value)
		)
	) {
	}
	
	template<typename T, BOUNDED_REQUIRES(
		is_valid_index(types<std::decay_t<T>>{}, types<Ts>{}...) and
		std::is_constructible_v<std::decay_t<T>, T &&> and
		std::is_move_assignable_v<std::decay_t<T>>
	)>
	constexpr void assignment(T && value) & noexcept(
		std::is_nothrow_constructible_v<std::decay_t<T>, T &&> and
		std::is_nothrow_move_assignable_v<std::decay_t<T>>
	) {
		visit(*this, [&](auto && original) {
			if constexpr (std::is_same_v<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = BOUNDED_FORWARD(value);
			} else {
				destroy(original);
				constexpr auto index = get_index(types<std::decay_t<T>>{}, types<Ts>{}...);
				m_data[1_bi] = variadic_union_t<Ts...>(index, std::move(value));
			}
		});
	}
	

	constexpr auto && get_function() const noexcept {
		return m_data[0_bi];
	}
	constexpr auto && get_function() noexcept {
		return m_data[0_bi];
	}


	constexpr auto index() const noexcept(noexcept(std::declval<GetFunction const &>()(std::declval<detail::variadic_union_t<Ts...> const &>()))) {
		return get_function()(m_data[1_bi]);
	}

	template<typename Index, typename... Args, typename = decltype(
		construct_return<typename decltype(get_type(Index{}, types<Ts>{}...))::type>(std::declval<Args>()...)
	)>
	constexpr auto & emplace(Index index, Args && ... args) & noexcept(noexcept(construct(std::declval<basic_variant_base &>()[index], BOUNDED_FORWARD(args)...))) {
		using indexed = typename decltype(get_type(index, types<Ts>{}...))::type;
		constexpr auto trivial = (... and (
			std::is_trivially_copy_constructible_v<Ts> and
			std::is_trivially_copy_assignable_v<Ts> and
			std::is_trivially_destructible_v<Ts>
		));
		constexpr auto index_value = get_index(index, types<Ts>{}...);
		if constexpr (std::is_nothrow_constructible_v<indexed, Args...>) {
			visit(*this, destroy);
			get_function() = GetFunction(index_value);
			if constexpr (trivial) {
				m_data[1_bi] = variadic_union_t<Ts...>(index_value, BOUNDED_FORWARD(args)...);
				return operator[](index);
			} else {
				return construct(operator[](index), BOUNDED_FORWARD(args)...);
			}
		} else {
			auto & ref = operator[](index);
			auto value = construct_return<std::decay_t<decltype(ref)>>(BOUNDED_FORWARD(args)...);
			visit(*this, destroy);
			get_function() = GetFunction(index_value);
			if constexpr (trivial) {
				m_data[1_bi] = variadic_union_t<Ts...>(index_value, std::move(value));
				return operator[](index);
			} else {
				return construct(ref, std::move(value));
			}
		}
	}

	template<typename Index, BOUNDED_REQUIRES(detail::is_valid_index(Index{}, types<Ts>{}...))>
	constexpr auto const & operator[](Index index_) const & noexcept {
		return operator_bracket(*this, index_);
	}
	template<typename Index, BOUNDED_REQUIRES(detail::is_valid_index(Index{}, types<Ts>{}...))>
	constexpr auto & operator[](Index index_) & noexcept {
		return operator_bracket(*this, index_);
	}
	template<typename Index, BOUNDED_REQUIRES(detail::is_valid_index(Index{}, types<Ts>{}...))>
	constexpr auto && operator[](Index index_) && noexcept {
		return operator_bracket(std::move(*this), index_);
	}

private:
	template<typename Variant, typename Index>
	static constexpr auto && operator_bracket(Variant && variant, Index const index_) noexcept {
		return ::bounded::detail::get_union_element(
			BOUNDED_FORWARD(variant).m_data[1_bi],
			get_index(index_, types<Ts>{}...)
		);
	}
	
	tuple<GetFunction, variadic_union_t<Ts...>> m_data;
};


}	// namespace detail
}	// namespace bounded
