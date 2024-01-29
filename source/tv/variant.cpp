// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module tv.variant;

import tv.insert;
import tv.get_index;
import tv.is_valid_index;
import tv.single_element_storage;
import tv.variadic_union;
import tv.variant_index;
import tv.visit;
import tv.indexed_value;

import bounded;
import std_module;

using namespace bounded::literal;

namespace tv {

template<typename T>
concept variant_copy_assignable = bounded::copy_constructible<T> and bounded::copy_assignable<T>;

template<typename T>
concept variant_trivially_copy_assignable = bounded::trivially_copy_constructible<T> and bounded::trivially_copy_assignable<T>;


template<typename T>
concept variant_move_assignable = bounded::move_constructible<T> and bounded::move_assignable<T>;

template<typename T>
concept variant_trivially_move_assignable = bounded::trivially_move_constructible<T> and bounded::trivially_move_assignable<T>;

constexpr auto equality_visitor = []
	<typename LHS, auto lhs_n, typename RHS, auto rhs_n>
	(indexed_value<LHS, lhs_n> const lhs, indexed_value<RHS, rhs_n> const rhs)
{
	if constexpr (lhs_n == rhs_n) {
		return lhs.value() == rhs.value();
	} else {
		return false;
	}
};

template<typename T>
constexpr auto single_equality_visitor(T const & single) {
	return [&]<typename U>(U const & element) -> bool {
		if constexpr (std::same_as<T, U>) {
			return element == single;
		} else {
			return false;
		}
	};
}

template<typename Function, typename... Ts>
concept unique_construct_function = matches_exactly_one_type<std::invoke_result_t<Function>, Ts...>;

template<typename Construct>
using constructed_type = std::decay_t<std::invoke_result_t<Construct>>;

struct copy_move_tag{};

export template<typename... Ts>
struct [[clang::trivial_abi]] variant {
private:
	template<typename Index>
	using type_at = typename decltype(::tv::get_type(Index(), bounded::type<Ts>...))::type;

public:
	constexpr variant(
		bounded::lazy_init_t,
		auto index_,
		bounded::construct_function_for<type_at<decltype(index_)>> auto && construct_
	):
		m_index(::tv::get_index(index_, bounded::type<Ts>...)),
		m_data(::tv::get_index(index_, bounded::type<Ts>...), OPERATORS_FORWARD(construct_))
	{
	}

	constexpr variant(auto index_, bounded::convertible_to<type_at<decltype(index_)>> auto && value):
		variant(
			bounded::lazy_init,
			index_,
			bounded::value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}

	constexpr variant(bounded::lazy_init_t, unique_construct_function<Ts...> auto && construct_):
		variant(
			bounded::lazy_init,
			bounded::type_t<constructed_type<decltype(construct_)>>(),
			OPERATORS_FORWARD(construct_)
		)
	{
	}
	
	constexpr variant(matches_exactly_one_type<Ts...> auto && value):
		variant(
			bounded::lazy_init,
			bounded::value_to_function(OPERATORS_FORWARD(value))
		)
	{
	}
	

	constexpr variant(variant const &) = default;

	constexpr variant(variant const & other) noexcept(
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) requires((... and bounded::copy_constructible<Ts>) and !(... and bounded::trivially_copy_constructible<Ts>)):
		variant(other, copy_move_tag{})
	{
	}


	constexpr variant(variant &&) = default;

	constexpr variant(variant && other) noexcept(
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) requires((... and bounded::move_constructible<Ts>) and !(... and bounded::trivially_move_constructible<Ts>)):
		variant(std::move(other), copy_move_tag{})
	{
	}

	constexpr auto operator=(variant const &) & -> variant & = default;

	constexpr auto operator=(variant const & other) & noexcept(
		(... and std::is_nothrow_copy_assignable_v<Ts>) and
		(... and std::is_nothrow_copy_constructible_v<Ts>)
	) -> variant & requires((... and variant_copy_assignable<Ts>) and !(... and variant_trivially_copy_assignable<Ts>)) {
		assign(other);
		return *this;
	}


	constexpr auto operator=(variant &&) & -> variant & = default;

	constexpr auto operator=(variant && other) & noexcept(
		(... and std::is_nothrow_move_assignable_v<Ts>) and
		(... and std::is_nothrow_move_constructible_v<Ts>)
	) -> variant & requires((... and variant_move_assignable<Ts>) and !(... and variant_trivially_move_assignable<Ts>)) {
		assign(std::move(other));
		return *this;
	}

	template<typename T> requires(
		(0_bi + ... + bounded::constant<std::same_as<std::decay_t<T>, Ts>>) == 1_bi and
		bounded::constructible_from<std::decay_t<T>, T &&> and
		std::is_assignable_v<std::decay_t<T> &, T &&>
	)
	constexpr auto & operator=(T && value) & {
		visit(*this, [&](auto & original) {
			if constexpr (std::same_as<std::decay_t<decltype(original)>, std::decay_t<T>>) {
				original = OPERATORS_FORWARD(value);
			} else {
				this->emplace_impl(
					bounded::type_t<std::decay_t<T>>(),
					bounded::value_to_function(OPERATORS_FORWARD(value)),
					[&] { bounded::destroy(original); }
				);
			}
		});
		return *this;
	}

	~variant() requires(... and bounded::trivially_destructible<Ts>) = default;
	constexpr ~variant() {
		visit(*this, bounded::destroy);
	}

	constexpr auto index() const -> variant_index<Ts...> {
		return m_index;
	}


	constexpr auto operator[](this auto && v, unique_type_identifier<Ts...> auto index_) -> auto && {
		return OPERATORS_FORWARD(v).m_data[
			::tv::get_index(index_, bounded::type<Ts>...)
		];
	}

	constexpr auto & emplace(auto index, bounded::construct_function_for<type_at<decltype(index)>> auto && construct_) & {
		return emplace_impl(index, OPERATORS_FORWARD(construct_), [&] { visit(*this, bounded::destroy); });
	}
	constexpr auto & emplace(unique_construct_function<Ts...> auto && construct_) & {
		return emplace(bounded::type_t<constructed_type<decltype(construct_)>>(), OPERATORS_FORWARD(construct_));
	}
	
	friend constexpr auto operator==(variant const & lhs, variant const & rhs) -> bool
		requires(... and bounded::equality_comparable<Ts>)
	{
		return visit_with_index(lhs, rhs, equality_visitor);
	}
	template<matches_exactly_one_type<Ts...> T>
	friend constexpr auto operator==(variant const & lhs, T const & rhs) -> bool
		requires(bounded::equality_comparable<T>)
	{
		return visit(lhs, ::tv::single_equality_visitor(rhs));
	}

private:
	constexpr auto & emplace_impl(auto index, bounded::construct_function_for<type_at<decltype(index)>> auto && construct_, auto && destroy_active) & {
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
		constexpr auto index_value = ::tv::get_index(index, bounded::type<Ts>...);
		m_index = variant_index<Ts...>(index_value);
		auto & data = bounded::construct_at(m_data, [&] { return variadic_union<Ts...>(index_value, OPERATORS_FORWARD(construct_)); });
		return data[index_value];
	}

	constexpr variant(auto && other, copy_move_tag):
		m_index(other.m_index),
		m_data(uninitialized_union())
	{
		visit_with_index(
			OPERATORS_FORWARD(other),
			[&](auto parameter) {
				replace_active_member(parameter.index, [&] -> decltype(auto) { return std::move(parameter).value(); });
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
					::tv::insert(*this, rhs.index, OPERATORS_FORWARD(rhs.value()));
					m_index = variant_index<Ts...>(rhs.index);
				}
			}
		);
	}

	[[no_unique_address]] variant_index<Ts...> m_index;
	[[no_unique_address]] variadic_union<Ts...> m_data;

	constexpr explicit variant(bounded::tombstone_tag, auto const make):
		m_index(make()),
		m_data(uninitialized_union())
	{
	}
	friend bounded::tombstone_traits<variant>;
	friend bounded::tombstone_traits_composer<&variant::m_index>;
};

} // namespace tv

template<typename... Ts>
struct bounded::tombstone_traits<tv::variant<Ts...>> : bounded::tombstone_traits_composer<&tv::variant<Ts...>::m_index> {
};
