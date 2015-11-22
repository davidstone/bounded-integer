// Optional class
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "forward_declaration.hpp"

#include "../noexcept.hpp"
#include "../requires.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T, bool is_trivially_destructible = std::is_trivially_destructible<T>::value>
struct default_optional_storage;

}	// namespace detail

struct in_place_t{};
constexpr auto in_place = in_place_t{};

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
struct default_optional_storage<T, true> {
	constexpr explicit default_optional_storage(optional_tag) noexcept:
		m_storage(),
		m_initialized(false) {
	}
	
	template<typename... Args>
	constexpr explicit default_optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_storage(in_place, std::forward<Args>(args)...),
		m_initialized(true) {
	}

	template<typename... Args>
	auto initialize(optional_tag, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value) {
		new(&m_storage.value) T(std::forward<Args>(args)...);
		m_initialized = true;
	}
	
	constexpr auto uninitialize(optional_tag) noexcept {
		// Trivially destructible, so we can skip the destructor call. That
		// allows this to be constexpr
		m_initialized = false;
	}

	
	constexpr auto is_initialized(optional_tag) const noexcept {
		return m_initialized;
	}
	
	constexpr operator T const & () const & noexcept {
		return m_storage.value;
	}
	constexpr operator T & () & noexcept {
		return m_storage.value;
	}
	constexpr operator T && () && noexcept {
		return std::move(m_storage.value);
	}
private:
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy(none) {
		}
		template<typename... Args>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
			value(std::forward<Args>(args)...) {
		}
		~underlying_storage() = default;

		none_t dummy;
		T value;
	};
	underlying_storage m_storage;
	bool m_initialized;
};


template<typename T>
struct default_optional_storage<T, false> {
	constexpr explicit default_optional_storage(optional_tag) noexcept:
		m_storage(),
		m_initialized(false) {
	}
	
	template<typename... Args>
	constexpr default_optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_storage(in_place, std::forward<Args>(args)...),
		m_initialized(true) {
	}
	
	template<typename... Args>
	auto initialize(optional_tag, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value) {
		uninitialize();
		new(&m_storage.value) T(std::forward<Args>(args)...);
		m_initialized = true;
	}
	
	auto uninitialize(optional_tag) noexcept {
		uninitialize();
	}

	constexpr auto is_initialized(optional_tag) const noexcept {
		return m_initialized;
	}

	constexpr operator T const & () const & noexcept {
		return m_storage.value;
	}
	constexpr operator T & () & noexcept {
		return m_storage.value;
	}
	constexpr operator T && () && noexcept {
		return std::move(m_storage.value);
	}

	~default_optional_storage() noexcept {
		uninitialize();
	}
	
private:
	auto uninitialize() noexcept {
		if (m_initialized) {
			m_storage.value.~T();
			m_initialized = false;
		}
	}
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy(none) {
		}
		template<typename... Args>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
			value(std::forward<Args>(args)...) {
		}
		~underlying_storage() {}

		none_t dummy;
		T value;
	};
	underlying_storage m_storage;
	bool m_initialized;
};

template<typename Optional, typename T>
constexpr auto & assign(Optional & target, T && source) noexcept(std::is_nothrow_constructible<typename Optional::value_type, T>::value and std::is_nothrow_assignable<typename Optional::value_type &, T>::value) {
	if (target) {
		*target = std::forward<T>(source);
	} else {
		target.emplace(std::forward<T>(source));
	}
	return target;
}

template<typename Target, typename Source>
constexpr auto & assign_from_optional(Target & target, Source && source) noexcept(noexcept(assign(target, *std::forward<Source>(source)))) {
	if (!source) {
		target = none;
	} else {
		assign(target, *std::forward<Source>(source));
	}
	return target;
}


}	// namespace detail

template<typename T>
struct optional {
private:
	struct common_init_tag{};
	// Cannot use std::is_constructible because that does not account for
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

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<value_type, Args && ...>::value)>
	constexpr explicit optional(in_place_t, Args && ... other) noexcept(std::is_nothrow_constructible<value_type, Args && ...>::value):
		m_value(std::forward<Args>(other)...) {
	}
	template<typename U, BOUNDED_REQUIRES(std::is_convertible<U &&, value_type>::value)>
	constexpr optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(in_place, std::forward<U>(other))) {
	}
	template<typename U, BOUNDED_REQUIRES(!std::is_convertible<U &&, value_type>::value and std::is_constructible<value_type, U &&>::value)>
	constexpr explicit optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(in_place, std::forward<U>(other))) {
	}


	constexpr optional(optional const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	constexpr optional(optional && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}

	#if 0
	// Not sure if these two constructors should exist or whether they should
	// have implicit versions in addition to the explicit verisons
	template<typename U>
	constexpr explicit optional(optional<U> const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	template<typename U>
	constexpr explicit optional(optional<U> && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}
	#endif

	
	constexpr auto operator*() const & -> value_type const & {
		return *this ? m_value : error_using_uninitialized_optional(m_value);
	}
	constexpr auto operator*() & -> value_type & {
		return *this ? m_value : error_using_uninitialized_optional(m_value);
	}
	constexpr auto operator*() && -> value_type && {
		return *this ? std::move(m_value) : error_using_uninitialized_optional(std::move(m_value));
	}

	constexpr auto operator->() const {
		return &operator*();
	}
	constexpr auto operator->() {
		return &operator*();
	}
	
	constexpr explicit operator bool() const noexcept {
		static_assert(noexcept(m_value.is_initialized(optional_tag{})));
		return m_value.is_initialized(optional_tag{});
	}

	// TODO: handle std::initializer_list
	template<typename... Args>
	constexpr auto emplace(Args && ... args) noexcept(std::is_nothrow_constructible<value_type, Args && ...>::value) {
		m_value.initialize(optional_tag{}, std::forward<Args>(args)...);
	}

	constexpr auto && operator=(none_t) & noexcept {
		static_assert(noexcept(m_value.uninitialize(optional_tag{})));
		m_value.uninitialize(optional_tag{});
		return *this;
	}
	constexpr auto && operator=(optional const & other) & BOUNDED_NOEXCEPT(
		detail::assign_from_optional(*this, other)
	)
	constexpr auto && operator=(optional && other) & BOUNDED_NOEXCEPT(
		detail::assign_from_optional(*this, std::move(other))
	)
	// TODO: make this work when value_type is a reference
	template<typename U, BOUNDED_REQUIRES(std::is_convertible<U &&, value_type>::value)>
	constexpr auto && operator=(U && other) & BOUNDED_NOEXCEPT(
		detail::assign(*this, std::forward<U>(other))
	)
	
private:
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// https://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<typename U>
	static auto && error_using_uninitialized_optional(U && u) {
		return std::forward<U>(u);
	}

	template<typename Optional>
	constexpr optional(Optional && other, common_init_tag) noexcept(std::is_nothrow_constructible<value_type, decltype(*std::declval<Optional>())>::value):
		optional(none) {
		if (other) {
			emplace(*std::forward<Optional>(other));
		}
	}
	
	optional_storage m_value;
};


template<typename T>
constexpr auto make_optional(T && value) noexcept -> optional<std::remove_cv_t<std::remove_reference_t<T>>> {
	return { std::forward<T>(value) };
}

}	// namespace bounded
