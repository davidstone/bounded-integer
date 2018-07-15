// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/tuple.hpp>
#include <containers/type.hpp>
#include <containers/variant/get_index.hpp>
#include <containers/variant/is_valid_index.hpp>
#include <containers/variant/variadic_union.hpp>
#include <containers/variant/visit.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename GetFunction, typename... Ts>
struct basic_variant_base {
	// TODO: assert GetFunction is noexcept

	template<typename F, typename Index, typename... Args, BOUNDED_REQUIRES(
		std::is_convertible_v<F, GetFunction> and
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)>
	constexpr basic_variant_base(F && function, Index index_, Args && ... args) noexcept(
		std::is_nothrow_constructible_v<GetFunction, F> and
		std::is_nothrow_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	):
		m_data(
			std::piecewise_construct,
			tie(std::forward<F>(function)),
			tie(get_index(index_, types<Ts>{}...), std::forward<Args>(args)...)
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
	constexpr basic_variant_base(Index const index_, Args && ... args) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_variant_base(
			GetFunction(get_index(index_, types<Ts>{}...)),
			index_,
			std::forward<Args>(args)...
		)
	) {
	}
	
	// This overload exists to mark this explicit
	template<typename Index, BOUNDED_REQUIRES(
		std::is_default_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type>
	)>
	constexpr explicit basic_variant_base(Index const index_) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_variant_base(
			GetFunction(get_index(index_, types<Ts>{}...)),
			index_
		)
	) {
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

	template<typename Index, typename... Args, BOUNDED_REQUIRES(
		std::is_constructible_v<typename decltype(get_type(Index{}, types<Ts>{}...))::type, Args...>
	)>
	constexpr auto & emplace(Index index, Args && ... args) & noexcept(noexcept(bounded::construct(std::declval<basic_variant_base &>()[index], std::forward<Args>(args)...))) {
		auto & ref = operator[](index);
		if constexpr (std::is_nothrow_constructible_v<typename decltype(get_type(index, types<Ts>{}...))::type, Args...>) {
			::containers::visit(*this, bounded::destroy);
			get_function() = GetFunction(get_index(index, types<Ts>{}...));
			return bounded::construct(ref, std::forward<Args>(args)...);
		} else {
			auto value = bounded::construct_return<std::decay_t<decltype(ref)>>(std::forward<Args>(args)...);
			::containers::visit(*this, bounded::destroy);
			get_function() = GetFunction(get_index(index));
			return bounded::construct(ref, std::move(value));
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
		return ::containers::detail::get_union_element(
			std::forward<Variant>(variant).m_data[1_bi],
			get_index(index_, types<Ts>{}...)
		);
	}
	
	tuple<GetFunction, variadic_union_t<Ts...>> m_data;
};


}	// namespace detail
}	// namespace containers
