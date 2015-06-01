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

#pragma once

#include "forward_declaration.hpp"

#include "../class.hpp"
#include "../enable_if.hpp"
#include "../numeric_limits.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace bounded {
namespace detail {

// This lets optional work with built-in types.
template<typename T>
struct get_underlying {
	using type = T;
};
template<intmax_t minimum, intmax_t maximum, typename overflow>
struct get_underlying<integer<minimum, maximum, overflow>> {
	using type = typename integer<minimum, maximum, overflow>::underlying_type;
};

template<typename T>
struct has_extra_space {
private:
	static_assert(basic_numeric_limits<T>::is_specialized, "Metafunction only works with integer types.");
	using underlying_type = typename get_underlying<T>::type;
	static constexpr intmax_t underlying_min = basic_numeric_limits<underlying_type>::min();
	static constexpr intmax_t underlying_max = basic_numeric_limits<underlying_type>::max();
	static constexpr intmax_t min = basic_numeric_limits<T>::min();
	static constexpr intmax_t max = basic_numeric_limits<T>::max();
public:
	static constexpr bool value = underlying_min < min or underlying_max > max;
};

template<typename T, bool has_extra_space>
struct optional_storage;

template<typename T>
struct optional_storage<T, true> {
private:
	using underlying_type = typename get_underlying<T>::type;
	static constexpr auto minimum = basic_numeric_limits<T>::min();
	static constexpr auto maximum = basic_numeric_limits<T>::max();
	static constexpr underlying_type uninitialized_value() noexcept {
		return static_cast<underlying_type>(minimum > basic_numeric_limits<underlying_type>::min() ? minimum - 1 : maximum + 1);
	}
	T m_value;
public:
	constexpr optional_storage() noexcept:
		m_value(uninitialized_value(), non_check) {
	}
	optional_storage(optional_storage const &) = default;
	optional_storage(optional_storage &&) = default;
	auto && operator=(optional_storage const & other) noexcept(noexcept(m_value = other.m_value)) {
		if (other.is_initialized()) {
			m_value = other.m_value;
		}
		else {
			m_value.unchecked_assignment(uninitialized_value());
		}
		return *this;
	}
	auto && operator=(optional_storage && other) noexcept(noexcept(m_value = other.m_value)) {
		if (other.is_initialized()) {
			m_value = other.m_value;
		}
		else {
			m_value.unchecked_assignment(uninitialized_value());
		}
		return *this;
	}
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

	constexpr auto value() const & noexcept -> T const & {
		return m_value;
	}
	auto value() & noexcept -> T & {
		return m_value;
	}
	constexpr auto value() && noexcept  -> T && {
		return std::move(m_value);
	}
	constexpr auto is_initialized() const noexcept {
		return m_value.value() != uninitialized_value();
	}
};

// Replaced this with a version that uses std::aligned_storage to allow it to
// work with any type
template<typename T>
struct optional_storage<T, false> {
	constexpr optional_storage() noexcept:
		m_initialized(false),
		m_value(uninitialized_value()) {
	}
	optional_storage(optional_storage const &) = default;
	optional_storage(optional_storage &&) = default;
	auto operator=(optional_storage const &) -> optional_storage & = default;
	auto operator=(optional_storage &&) -> optional_storage & = default;
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

	constexpr auto value() const & noexcept  -> T const & {
		return m_value;
	}
	auto value() & noexcept -> T & {
		return m_value;
	}
	constexpr auto value() && noexcept -> T && {
		return std::move(m_value);
	}
	constexpr auto is_initialized() const noexcept {
		return m_initialized;
	}
private:
	using underlying_type = typename get_underlying<T>::type;
	static constexpr auto uninitialized_value() noexcept {
		return static_cast<underlying_type>(basic_numeric_limits<T>::min());
	}
	bool m_initialized;
	T m_value;
};

}	// namespace detail

template<typename T>
struct optional {
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

	auto operator=(optional const &) -> optional & = default;
	auto operator=(optional &&) -> optional & = default;
	template<typename U>
	auto && operator=(U && other) noexcept(noexcept(std::declval<optional &>() = optional(std::forward<U>(other)))) {
		*this = optional(std::forward<U>(other));
		return *this;
	}
	
	// TODO: conform better to std::optional
	// comma operator is used to avoid binding the reference to a temporary
	constexpr auto && value() const & {
		return m_storage.is_initialized() ? m_storage.value() : (throw std::logic_error("bad optional access"), m_storage.value());
	}
	auto && value() & {
		return m_storage.is_initialized() ? m_storage.value() : (throw std::logic_error("bad optional access"), m_storage.value());
	}
	auto && value() && {
		return m_storage.is_initialized() ? std::move(m_storage).value() : (throw std::logic_error("bad optional access"), std::move(m_storage).value());
	}
	constexpr auto && operator*() const & {
		return value();
	}
	auto && operator*() & {
		return value();
	}
	auto && operator*() && {
		return value();
	}
	
	constexpr auto operator->() const {
		return &value();
	}
	auto operator->() {
		return &value();
	}
	
	constexpr decltype(auto) get_value_or(value_type const & default_value) const noexcept {
		return m_storage.is_initialized() ? m_storage.value() : default_value;
	}
	
	constexpr explicit operator bool() const noexcept {
		return m_storage.is_initialized();
	}
	constexpr auto operator!() const noexcept {
		return !m_storage.is_initialized();
	}
private:
	template<typename U>
	friend struct optional;
	storage_type m_storage;
};


template<typename T>
auto make_optional(T && value) noexcept -> optional<std::remove_reference_t<T>> {
	return { std::forward<T>(value) };
}

}	// namespace bounded
