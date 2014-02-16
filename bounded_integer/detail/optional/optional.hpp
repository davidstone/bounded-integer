// Optional class
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_DETAIL_OPTIONAL_OPTIONAL_HPP_
#define BOUNDED_INTEGER_DETAIL_OPTIONAL_OPTIONAL_HPP_

#include "forward_declaration.hpp"

#include "../class.hpp"
#include "../enable_if.hpp"
#include "../numeric_limits.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace bounded_integer {
namespace detail {

template<typename T>
class has_extra_space {
private:
	static_assert(std::numeric_limits<T>::is_specialized, "Metafunction only works with integer types.");
	using underlying_type = typename T::underlying_type;
	static constexpr intmax_t underlying_min = std::numeric_limits<underlying_type>::min();
	static constexpr intmax_t underlying_max = std::numeric_limits<underlying_type>::max();
	static constexpr intmax_t min = static_cast<intmax_t>(std::numeric_limits<T>::min());
	static constexpr intmax_t max = static_cast<intmax_t>(std::numeric_limits<T>::max());
public:
	static constexpr bool value = underlying_min < min or underlying_max > max;
};

template<typename T, bool has_extra_space>
class optional_storage;

template<typename T>
class optional_storage<T, true> {
public:
	constexpr optional_storage() noexcept:
		m_value(uninitialized_value(), non_check) {
	}
	optional_storage(optional_storage const &) = default;
	optional_storage(optional_storage &&) = default;
	optional_storage & operator=(optional_storage const &) = default;
	optional_storage & operator=(optional_storage &&) = default;
	template<typename U, enable_if_t<std::is_convertible<U &&, T>::value> = clang_dummy>
	constexpr optional_storage(U && other) noexcept(std::is_nothrow_constructible<T, U &&>::value):
		m_value(std::forward<U>(other)) {
	}
	template<typename ... Args, enable_if_t<std::is_constructible<T, Args && ...>::value> = clang_dummy>
	constexpr explicit optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_value(std::forward<Args>(args)...) {
	}
	
	template<typename U, bool extra_space, enable_if_t<std::is_constructible<T, U const &>::value> = clang_dummy>
	constexpr explicit optional_storage(optional_storage<U, extra_space> const & other) noexcept(std::is_nothrow_constructible<T, U const &>::value):
		m_value(other.is_initialized() ? other.value() : uninitialized_value()) {
	}
	template<typename U, bool extra_space, enable_if_t<std::is_constructible<T, U &&>::value> = clang_dummy>
	constexpr explicit optional_storage(optional_storage<U, extra_space> && other) noexcept(std::is_nothrow_constructible<T, U &&>::value):
		m_value(other.is_initialized() ? std::move(other.value()) : uninitialized_value()) {
	}

	constexpr T const & value() const & noexcept {
		return m_value;
	}
	T & value() & noexcept {
		return m_value;
	}
	constexpr T && value() && noexcept {
		return std::move(m_value);
	}
	constexpr bool is_initialized() const noexcept {
		return m_value != uninitialized_value();
	}
private:
	using underlying_type = typename T::underlying_type;
	static constexpr auto minimum = static_cast<intmax_t>(std::numeric_limits<T>::min());
	static constexpr auto maximum = static_cast<intmax_t>(std::numeric_limits<T>::max());
	static constexpr underlying_type uninitialized_value() noexcept {
		return static_cast<underlying_type>(minimum > std::numeric_limits<underlying_type>::min() ? minimum - 1 : maximum + 1);
	}
	T m_value;
};

// Replaced this with a version that uses std::aligned_storage to allow it to
// work with any type
template<typename T>
class optional_storage<T, false> {
public:
	constexpr optional_storage() noexcept:
		m_initialized(false),
		m_value(uninitialized_value()) {
	}
	optional_storage(optional_storage const &) = default;
	optional_storage(optional_storage &&) = default;
	optional_storage & operator=(optional_storage const &) = default;
	optional_storage & operator=(optional_storage &&) = default;
	template<typename U, enable_if_t<std::is_convertible<U &&, T>::value> = clang_dummy>
	constexpr optional_storage(U && other) noexcept(std::is_nothrow_constructible<T, U &&>::value):
		m_initialized(true),
		m_value(std::forward<U>(other)) {
	}
	template<typename ... Args, enable_if_t<std::is_constructible<T, Args && ...>::value> = clang_dummy>
	constexpr explicit optional_storage(Args && ... args) noexcept(std::is_nothrow_constructible<T, Args && ...>::value):
		m_initialized(true),
		m_value(std::forward<Args>(args)...) {
	}
	
	template<typename U, bool extra_space, enable_if_t<std::is_constructible<T, U const &>::value> = clang_dummy>
	constexpr explicit optional_storage(optional_storage<U, extra_space> const & other) noexcept(std::is_nothrow_constructible<T, U const &>::value):
		m_initialized(other.is_initialized()),
		m_value(m_initialized ? other.value() : uninitialized_value()) {
	}
	template<typename U, bool extra_space, enable_if_t<std::is_constructible<T, U &&>::value> = clang_dummy>
	constexpr explicit optional_storage(optional_storage<U, extra_space> && other) noexcept(std::is_nothrow_constructible<T, U &&>::value):
		m_initialized(other.is_initialized()),
		m_value(m_initialized ? std::move(other.value()) : uninitialized_value()) {
	}

	constexpr T const & value() const & noexcept {
		return m_value;
	}
	T & value() & noexcept {
		return m_value;
	}
	constexpr T && value() && noexcept {
		return std::move(m_value);
	}
	constexpr bool is_initialized() const noexcept {
		return m_initialized;
	}
private:
	using underlying_type = typename T::underlying_type;
	static constexpr underlying_type uninitialized_value() noexcept {
		return static_cast<underlying_type>(std::numeric_limits<T>::min());
	}
	bool m_initialized;
	T m_value;
};

}	// namespace detail

template<typename T>
class optional {
public:
	using value_type = T;
private:
	using storage_type = detail::optional_storage<value_type, detail::has_extra_space<value_type>::value>;
public:
	constexpr optional() noexcept {}
	constexpr optional(none_t) noexcept:
		optional{} {
	}

	constexpr optional(optional const &) noexcept = default;
	constexpr optional(optional &&) noexcept = default;

	template<typename U>
	constexpr explicit optional(optional<U> const & other):
		m_storage(other.m_storage) {
	}
	template<typename U>
	constexpr explicit optional(optional<U> && other):
		m_storage(std::move(other.m_storage)) {
	}

	template<typename U, enable_if_t<std::is_convertible<U &&, value_type>::value> = clang_dummy>
	constexpr optional(U && other) noexcept(std::is_nothrow_constructible<storage_type, U &&>::value):
		m_storage(std::forward<U>(other)) {
	}
	template<typename ... Args, enable_if_t<std::is_constructible<value_type, Args && ...>::value> = clang_dummy>
	constexpr explicit optional(Args && ... args) noexcept(std::is_nothrow_constructible<storage_type, Args && ...>::value):
		m_storage(std::forward<Args>(args)...) {
	}

	optional & operator=(optional const &) noexcept = default;
	optional & operator=(optional &&) noexcept = default;
	template<typename U>
	optional & operator=(U && other) noexcept(noexcept(std::declval<optional &>() = optional(std::forward<U>(other)))) {
		*this = optional(std::forward<U>(other));
		return *this;
	}
	
	// TODO: conform better to std::optional
	// comma operator is used to avoid binding the reference to a temporary
	constexpr value_type const & value() const & {
		return m_storage.is_initialized() ? m_storage.value() : (throw std::logic_error("bad optional access"), m_storage.value());
	}
	value_type & value() & {
		return m_storage.is_initialized() ? m_storage.value() : (throw std::logic_error("bad optional access"), m_storage.value());
	}
	value_type && value() && {
		return m_storage.is_initialized() ? std::move(m_storage).value() : (throw std::logic_error("bad optional access"), std::move(m_storage).value());
	}
	constexpr value_type const & operator*() const & {
		return value();
	}
	value_type & operator*() & {
		return value();
	}
	value_type && operator*() && {
		return std::move(value());
	}
	
	constexpr value_type const * operator->() const {
		return &value();
	}
	value_type * operator->() {
		return &value();
	}
	
	constexpr value_type const & get_value_or(value_type const & default_value) const noexcept {
		return m_storage.is_initialized() ? m_storage.value() : default_value;
	}
	
	constexpr explicit operator bool() const noexcept {
		return m_storage.is_initialized();
	}
	constexpr bool operator!() const noexcept {
		return !m_storage.is_initialized();
	}
private:
	template<typename U>
	friend class optional;
	storage_type m_storage;
};


template<typename T>
optional<typename std::remove_reference<T>::type> make_optional(T && value) noexcept {
	return { std::forward<T>(value) };
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_DETAIL_OPTIONAL_OPTIONAL_HPP_
