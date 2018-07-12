// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/optional/forward_declaration.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/requires.hpp>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {

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

template<typename T, bool is_trivially_destructible = std::is_trivially_destructible_v<T>>
struct default_optional_storage_base;

template<typename T>
struct default_optional_storage_base<T, true> {
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy(none) {
		}
		template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args && ...>)>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
			value(std::forward<Args>(args)...) {
		}
		template<typename... Args, BOUNDED_REQUIRES(!std::is_constructible_v<T, Args && ...>)>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(noexcept(underlying_storage{std::forward<Args>(args)...})):
			value{std::forward<Args>(args)...} {
		}
		~underlying_storage() = default;

		none_t dummy;
		T value;
	};
	
	underlying_storage storage;
	bool initialized;
};

template<typename T>
struct default_optional_storage_base<T, false> {
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy(none) {
		}
		template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<T, Args && ...>)>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
			value(std::forward<Args>(args)...) {
		}
		template<typename... Args, BOUNDED_REQUIRES(!std::is_constructible_v<T, Args && ...>)>
		constexpr underlying_storage(in_place_t, Args && ... args) noexcept(noexcept(underlying_storage{std::forward<Args>(args)...})):
			value{std::forward<Args>(args)...} {
		}
		~underlying_storage() {}

		none_t dummy;
		T value;
	};
	
	~default_optional_storage_base() noexcept {
		if (initialized) {
			destroy(storage.value);
		}
	}
	
	underlying_storage storage;
	bool initialized;
};


template<typename T>
struct default_optional_storage {
	constexpr explicit default_optional_storage(optional_tag) noexcept:
		m_data{
			{},
			false
		} {
	}
	
	template<typename... Args>
	constexpr default_optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		m_data{
			{ in_place, std::forward<Args>(args)... },
			true
		} {
	}
	
	template<typename... Args>
	constexpr auto initialize(optional_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>) {
		uninitialize();
		construct(m_data.storage.value, std::forward<Args>(args)...);
		m_data.initialized = true;
	}
	
	constexpr auto uninitialize(optional_tag) noexcept {
		uninitialize();
	}

	constexpr auto is_initialized(optional_tag) const noexcept {
		return m_data.initialized;
	}

	constexpr operator T const & () const & noexcept {
		return m_data.storage.value;
	}
	constexpr operator T & () & noexcept {
		return m_data.storage.value;
	}
	constexpr operator T && () && noexcept {
		return std::move(m_data.storage.value);
	}

private:
	constexpr auto uninitialize() noexcept {
		if (m_data.initialized) {
			::bounded::destroy(m_data.storage.value);
			m_data.initialized = false;
		}
	}
	default_optional_storage_base<T> m_data;
};

template<typename Optional, typename T>
constexpr auto & assign(Optional & target, T && source) noexcept(std::is_nothrow_constructible_v<typename Optional::value_type, T> and std::is_nothrow_assignable_v<typename Optional::value_type &, T>) {
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

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible_v<value_type, Args && ...>)>
	constexpr explicit optional(in_place_t, Args && ... other) noexcept(std::is_nothrow_constructible_v<value_type, Args && ...>):
		m_value(std::forward<Args>(other)...) {
	}
	template<typename U, BOUNDED_REQUIRES(std::is_convertible_v<U &&, value_type>)>
	constexpr optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(in_place, std::forward<U>(other))) {
	}
	template<typename U, BOUNDED_REQUIRES(!std::is_convertible_v<U &&, value_type> and std::is_constructible_v<value_type, U &&>)>
	constexpr explicit optional(U && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(in_place, std::forward<U>(other))) {
	}


	constexpr optional(optional const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	constexpr optional(optional && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}

	template<typename U, BOUNDED_REQUIRES(std::is_convertible_v<U const &, value_type>)>
	constexpr optional(optional<U> const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	template<typename U, BOUNDED_REQUIRES(std::is_convertible_v<U &&, value_type>)>
	constexpr optional(optional<U> && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}
	template<typename U, BOUNDED_REQUIRES(!std::is_convertible_v<U const &, value_type> and std::is_constructible_v<value_type, U const &>)>
	constexpr explicit optional(optional<U> const & other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(other, common_init_tag{})) {
	}
	template<typename U, BOUNDED_REQUIRES(!std::is_convertible_v<U &&, value_type> and std::is_constructible_v<value_type, U &&>)>
	constexpr explicit optional(optional<U> && other)
		BOUNDED_NOEXCEPT_INITIALIZATION(optional(std::move(other), common_init_tag{})) {
	}

	
	constexpr auto operator*() const & noexcept -> value_type const & {
		return *this ? m_value : error_using_uninitialized_optional(m_value);
	}
	constexpr auto operator*() & noexcept -> value_type & {
		return *this ? m_value : error_using_uninitialized_optional(m_value);
	}
	constexpr auto operator*() && noexcept -> value_type && {
		return *this ? std::move(m_value) : error_using_uninitialized_optional(std::move(m_value));
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
		m_value.initialize(optional_tag{}, std::forward<Args>(args)...);
	}

	// Cannot use BOUNDED_NOEXCEPT because of gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869
	constexpr auto && operator=(none_t) & noexcept {
		static_assert(noexcept(m_value.uninitialize(optional_tag{})));
		m_value.uninitialize(optional_tag{});
		return *this;
	}
	constexpr auto && operator=(optional const & other) & noexcept(noexcept(detail::assign_from_optional(std::declval<optional &>(), other))) {
		return detail::assign_from_optional(*this, other);
	}
	constexpr auto && operator=(optional && other) & noexcept(noexcept(detail::assign_from_optional(std::declval<optional& >(), std::move(other)))) {
		return detail::assign_from_optional(*this, std::move(other));
	}
	// TODO: make this work when value_type is a reference
	template<typename U, BOUNDED_REQUIRES(std::is_convertible_v<U &&, value_type>)>
	constexpr auto && operator=(U && other) & noexcept(noexcept(detail::assign(std::declval<optional &>(), std::forward<U>(other)))) {
		return detail::assign(*this, std::forward<U>(other));
	}
	
private:
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// https://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<typename U>
	static auto && error_using_uninitialized_optional(U && u) noexcept {
		return std::forward<U>(u);
	}

	template<typename Optional>
	constexpr optional(Optional && other, common_init_tag) noexcept(std::is_nothrow_constructible<value_type, decltype(*std::declval<Optional>())>{}):
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
