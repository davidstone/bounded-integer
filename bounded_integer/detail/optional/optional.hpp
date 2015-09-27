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

#include "../noexcept.hpp"
#include "../requires.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bounded {

// none_t cannot be default constructible or we get an ambiguity in op = {};
struct none_t {
	constexpr none_t(int) noexcept {}
};
constexpr auto none = none_t(0);
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

template<typename T>
struct optional_storage : default_optional_storage<T> {
private:
	using base = default_optional_storage<T>;
public:
	using base::base;
};

namespace detail {

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
public:
	using value_type = T;

	constexpr optional(none_t = none) noexcept {}

	template<typename... Args, BOUNDED_REQUIRES(std::is_constructible<value_type, Args && ...>::value)>
	constexpr explicit optional(in_place_t, Args && ... other) noexcept(std::is_nothrow_constructible<optional_storage<value_type>, in_place_t, Args && ...>::value):
		m_storage(in_place, std::forward<Args>(other)...) {
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
	
	constexpr explicit operator bool() const noexcept {
		return m_storage.is_initialized();
	}

	constexpr auto && operator=(none_t) & noexcept {
		m_storage.reset();
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
	
	template<typename Optional>
	constexpr optional(Optional && other, common_init_tag) noexcept(std::is_nothrow_constructible<value_type, optional_value_type<Optional>>::value):
		optional(none) {
		if (other) {
			m_storage.emplace(*std::forward<Optional>(other));
		}
	}
	
	optional_storage<value_type> m_storage;
};


template<typename T>
constexpr auto make_optional(T && value) noexcept -> optional<std::remove_cv_t<std::remove_reference_t<T>>> {
	return { std::forward<T>(value) };
}

}	// namespace bounded
