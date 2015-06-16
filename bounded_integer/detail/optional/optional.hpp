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

#include "../enable_if.hpp"
#include "../noexcept.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {

struct none_t{};
constexpr auto none = none_t{};
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
// The last two cases are handled generally. Users can specialize
// bounded::optional_storage to opt-in to the first behavior.
//
// default_optional_storage can then be used to fall back to the default case
// for certain versions of class templates. See specialization.hpp for an
// example of this


template<typename T, bool is_trivially_destructible = std::is_trivially_destructible<T>::value>
struct default_optional_storage;

template<typename T>
struct default_optional_storage<T, true> {
	constexpr default_optional_storage() noexcept:
		m_storage{},
		m_initialized(false) {
	}
	
	template<typename... Args>
	constexpr explicit default_optional_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_storage(in_place, std::forward<Args>(args)...),
		m_initialized(true) {
	}

	template<typename... Args>
	void emplace(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value) {
		new(&m_storage.value) T(std::forward<Args>(args)...);
		m_initialized = true;
	}
	
	constexpr void reset() noexcept {
		m_initialized = false;
	}

	
	constexpr auto is_initialized() const noexcept {
		return m_initialized;
	}
	constexpr auto && value() const & noexcept {
		return m_storage.value;
	}
	constexpr auto && value() & noexcept {
		return m_storage.value;
	}
	constexpr auto && value() && noexcept {
		return std::move(m_storage.value);
	}
private:
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy{} {
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
private:
	using base = default_optional_storage<T, true>;
public:
	constexpr default_optional_storage() noexcept:
		m_storage{},
		m_initialized(false) {
	}
	
	template<typename... Args>
	constexpr default_optional_storage(in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_storage(in_place, std::forward<Args>(args)...),
		m_initialized(true) {
	}
	
	template<typename... Args>
	void emplace(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value) {
		reset();
		new(&m_storage.value) T(std::forward<Args>(args)...);
		m_initialized = true;
	}
	
	void reset() noexcept {
		if (m_initialized) {
			value().~T();
			m_initialized = false;
		}
	}

	constexpr auto is_initialized() const noexcept {
		return m_initialized;
	}
	constexpr auto && value() const & noexcept {
		return m_storage.value;
	}
	constexpr auto && value() & noexcept {
		return m_storage.value;
	}
	constexpr auto && value() && noexcept {
		return std::move(m_storage.value);
	}

	~default_optional_storage() noexcept {
		reset();
	}
	
private:
	union underlying_storage {
		constexpr underlying_storage() noexcept:
			dummy{} {
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

template<typename T>
struct optional_storage : default_optional_storage<T> {
private:
	using base = default_optional_storage<T>;
public:
	using base::base;
};

struct bad_optional_access : std::logic_error {
private:
	using base = std::logic_error;
public:
	using base::base;
};

template<typename T>
struct optional {
	using value_type = T;

	constexpr optional(none_t) noexcept:
		optional{} {
	}
	template<typename U, enable_if_t<std::is_convertible<U &&, value_type>::value> = clang_dummy>
	constexpr optional(U && other) noexcept(std::is_nothrow_constructible<value_type, U &&>::value):
		m_storage(in_place, std::forward<U>(other)) {
	}
	// This handles default construction and in-place construction
	template<typename... Args, enable_if_t<std::is_constructible<optional_storage<value_type>, Args && ...>::value> = clang_dummy>
	constexpr explicit optional(Args && ... other) noexcept(std::is_nothrow_constructible<optional_storage<value_type>, Args && ...>::value):
		m_storage(std::forward<Args>(other)...) {
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

	
	constexpr auto && operator*() const & {
		return *this ? m_storage.value() : error_using_uninitialized_optional(m_storage.value());
	}
	constexpr auto && operator*() & {
		return *this ? m_storage.value() : error_using_uninitialized_optional(m_storage.value());
	}
	constexpr auto && operator*() && {
		return *this ? std::move(m_storage).value() : error_using_uninitialized_optional(std::move(m_storage).value());
	}

	constexpr auto operator->() const {
		return &operator*();
	}
	constexpr auto operator->() {
		return &operator*();
	}
	
	// comma operator is used to avoid binding the reference to a temporary
	constexpr auto && value() const & {
		return *this ? **this : (throw std::logic_error("bad optional access"), **this);
	}
	constexpr auto && value() & {
		return *this ? **this : (throw std::logic_error("bad optional access"), **this);
	}
	constexpr auto && value() && {
		return *this ? *std::move(*this) : (throw std::logic_error("bad optional access"), *std::move(*this));
	}
	
	// TODO: Make this a free function
	template<typename U>
	constexpr auto value_or(U && default_value) const & {
		return *this ? **this : static_cast<value_type>(std::forward<U>(default_value));
	}
	template<typename U>
	constexpr auto value_or(U && default_value) && {
		return *this ? *std::move(*this) : static_cast<value_type>(std::forward<U>(default_value));
	}
	
	constexpr explicit operator bool() const noexcept {
		return m_storage.is_initialized();
	}

	constexpr auto && operator=(none_t) & noexcept {
		m_storage.reset();
		return *this;
	}
	constexpr auto && operator=(optional const & other) & BOUNDED_NOEXCEPT(
		assign_from_optional(other)
	)
	constexpr auto && operator=(optional && other) & BOUNDED_NOEXCEPT(
		assign_from_optional(std::move(other))
	)
	// TODO: make this work when value_type is a reference
	template<typename U, enable_if_t<std::is_same<std::decay_t<U>, value_type>::value>>
	constexpr auto && operator=(U && other) & noexcept(is_nothrow_optional_assignable<U &&>) {
		return assign(std::forward<U>(other));
	}
	
	// TODO: handle std::initializer_list
	template<typename... Args>
	constexpr void emplace(Args && ... args) noexcept(std::is_nothrow_constructible<value_type, Args && ...>::value) {
		m_storage.emplace(std::forward<Args>(args)...);
	}
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

	// Handles value category correctly
	template<typename Optional>
	using optional_value_type = decltype(*std::declval<Optional>());
	
	struct common_init_tag{};
	template<typename Optional>
	constexpr optional(Optional && other, common_init_tag) noexcept(std::is_nothrow_constructible<value_type, optional_value_type<Optional>>::value):
		optional(none) {
		if (other) {
			m_storage.emplace(*std::forward<Optional>(other));
		}
	}
	
	template<typename U>
	static constexpr bool is_nothrow_optional_assignable = std::is_nothrow_constructible<value_type, U>::value and std::is_nothrow_assignable<value_type &, U>::value;
	
	template<typename Optional>
	constexpr auto && assign_from_optional(Optional && other) & noexcept(is_nothrow_optional_assignable<optional_value_type<Optional>>) {
		if (!other) {
			*this = none;
		} else {
			assign(*std::forward<Optional>(other));
		}
		return *this;
	}
	template<typename U>
	constexpr auto && assign(U && other) & noexcept(is_nothrow_optional_assignable<U &&>) {
		auto & self = *this;
		if (self) {
			*self = std::forward<U>(other);
		} else {
			emplace(std::forward<U>(other));
		}
		return *this;
	}
	
	optional_storage<value_type> m_storage;
};


template<typename T>
constexpr auto make_optional(T && value) noexcept -> optional<std::remove_reference_t<T>> {
	return { std::forward<T>(value) };
}

}	// namespace bounded
