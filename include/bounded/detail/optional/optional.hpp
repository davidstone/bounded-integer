// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/optional/forward_declaration.hpp>

#include <bounded/detail/variant/variant.hpp>

#include <bounded/assert.hpp>
#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/detail/noexcept.hpp>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {

// There are three general classifications of objects that can be stored in an optional:
//
// nullable: The object has the ability to store a null state within itself. For
// instance, a bounded::integer that has a narrower range than its underlying
// type can store a value outside of its range to indicate the null state.
//
// non-nullable, but trivially_destructible: The object has a trivial
// destructor, allowing optional<T> to be a literal type
//
// Any other type
//
// The last two cases are handled generally. For a type to opt-in to a
// space-efficient representation, it needs the following member functions:
//
//	* `T(optional_tag)` constructs an uninitialized value
//	* `initialize(optional_tag, T && ...)` constructs an object when there may
//		be one in existence already
//	* `uninitialize(optional_tag)` destroys the contained object
//	* `is_initialized(optional_tag)` checks whether the object is currently in
//		an initialized state


namespace detail {

template<typename T>
struct default_optional_storage {
	constexpr explicit default_optional_storage(optional_tag) noexcept:
		m_data(std::in_place, none_index, none)
	{
	}
	
	template<typename... Args>
	constexpr default_optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		m_data(std::in_place, value_index, BOUNDED_FORWARD(args)...)
	{
	}
	
	template<typename... Args>
	constexpr auto initialize(optional_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>) {
		m_data.emplace(value_index, BOUNDED_FORWARD(args)...);
	}
	
	constexpr auto uninitialize(optional_tag) noexcept {
		m_data.emplace(none_index, none);
	}

	constexpr auto is_initialized(optional_tag) const noexcept {
		return m_data.index() == value_index;
	}

	constexpr operator T const & () const & noexcept {
		return m_data[value_index];
	}
	constexpr operator T & () & noexcept {
		return m_data[value_index];
	}
	constexpr operator T && () && noexcept {
		return std::move(m_data)[value_index];
	}

private:
	static constexpr auto none_index = constant<0>;
	static constexpr auto value_index = constant<1>;
	variant<none_t, T> m_data;
};

template<typename Optional, typename T>
constexpr auto & assign(Optional & target, T && source) noexcept(std::is_nothrow_constructible_v<typename Optional::value_type, T> and std::is_nothrow_assignable_v<typename Optional::value_type &, T>) {
	if (target) {
		*target = BOUNDED_FORWARD(source);
	} else {
		target.emplace(BOUNDED_FORWARD(source));
	}
	return target;
}

template<typename Target, typename Source>
constexpr auto & assign_from_optional(Target & target, Source && source) noexcept(noexcept(assign(target, *BOUNDED_FORWARD(source)))) {
	if (!source) {
		target = none;
	} else {
		assign(target, *BOUNDED_FORWARD(source));
	}
	return target;
}


}	// namespace detail

template<typename T>
struct optional {
private:
	struct common_init_tag{};
	// Cannot use concepts or std::is_constructible because this could require
	// friendship
	template<typename U>
	static constexpr auto is_specialized(decltype(U(std::declval<optional_tag>())) *) noexcept { return true; }
	template<typename>
	static constexpr auto is_specialized(...) noexcept { return false; }
	using optional_storage = std::conditional_t<is_specialized<T>(nullptr), T, detail::default_optional_storage<T>>;
public:
	using value_type = T;

	constexpr optional(none_t = none) noexcept:
		m_value(optional_tag{})
	{
		static_assert(noexcept(optional_storage(optional_tag{})));
	}

	template<typename... Args> requires std::is_constructible_v<value_type, Args && ...>
	constexpr explicit optional(std::in_place_t, Args && ... other) noexcept(std::is_nothrow_constructible_v<value_type, Args && ...>):
		m_value(BOUNDED_FORWARD(other)...) {
	}
	template<typename U> requires std::is_convertible_v<U &&, value_type>
	constexpr optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::in_place, BOUNDED_FORWARD(other))) {
	}
	template<typename U> requires (!std::is_convertible_v<U &&, value_type> and std::is_constructible_v<value_type, U &&>)
	constexpr explicit optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::in_place, BOUNDED_FORWARD(other))) {
	}


	template<typename U> requires std::is_convertible_v<U const &, value_type>
	constexpr optional(optional<U> const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	template<typename U> requires std::is_convertible_v<U &&, value_type>
	constexpr optional(optional<U> && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}
	template<typename U> requires (!std::is_convertible_v<U const &, value_type> and std::is_constructible_v<value_type, U const &>)
	constexpr explicit optional(optional<U> const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	template<typename U> requires (!std::is_convertible_v<U &&, value_type> and std::is_constructible_v<value_type, U &&>)
	constexpr explicit optional(optional<U> && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}

	
	constexpr auto operator*() const & noexcept -> value_type const & {
		BOUNDED_ASSERT(*this);
		return m_value;
	}
	constexpr auto operator*() & noexcept -> value_type & {
		BOUNDED_ASSERT(*this);
		return m_value;
	}
	constexpr auto operator*() && noexcept -> value_type && {
		BOUNDED_ASSERT(*this);
		return std::move(m_value);
	}

	constexpr auto operator->() const noexcept {
		return &operator*();
	}
	constexpr auto operator->() noexcept {
		return &operator*();
	}
	
	constexpr explicit operator bool() const noexcept {
		static_assert(noexcept(m_value.is_initialized(optional_tag{})));
		return m_value.is_initialized(optional_tag{});
	}

	// TODO: handle std::initializer_list
	template<typename... Args>
	constexpr auto emplace(Args && ... args) noexcept(std::is_nothrow_constructible_v<value_type, Args && ...>) {
		m_value.initialize(optional_tag{}, BOUNDED_FORWARD(args)...);
	}

	// Cannot use BOUNDED_NOEXCEPT because of gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869
	constexpr auto && operator=(none_t) & noexcept {
		static_assert(noexcept(m_value.uninitialize(optional_tag{})));
		m_value.uninitialize(optional_tag{});
		return *this;
	}
	// TODO: make this work when value_type is a reference
	template<typename U> requires(std::is_convertible_v<U &&, value_type>)
	constexpr auto && operator=(U && other) & noexcept(noexcept(detail::assign(std::declval<optional &>(), BOUNDED_FORWARD(other)))) {
		return detail::assign(*this, BOUNDED_FORWARD(other));
	}
	
private:

	template<typename Optional>
	constexpr optional(Optional && other, common_init_tag) noexcept(std::is_nothrow_constructible<value_type, decltype(*std::declval<Optional>())>{}):
		optional(none) {
		if (other) {
			emplace(*BOUNDED_FORWARD(other));
		}
	}
	
	optional_storage m_value;
};

template<typename T> requires(!std::is_same_v<T, none_t> and !std::is_same_v<T, std::in_place_t>)
optional(T) -> optional<T>;

template<typename T>
constexpr auto make_optional(T && value) noexcept -> optional<std::remove_cv_t<std::remove_reference_t<T>>> {
	return { BOUNDED_FORWARD(value) };
}

}	// namespace bounded
