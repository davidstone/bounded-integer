// Specialization for optional
// Copyright (C) 2013 David Stone
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

#ifndef RANGED_INTEGER_DETAIL_OPTIONAL_HPP_
#define RANGED_INTEGER_DETAIL_OPTIONAL_HPP_

#include "class.hpp"
#include "enable_if.hpp"
#include "optional_forward.hpp"
#include "optional_relational_operators.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>

#include <boost/assert.hpp>

namespace detail {

template<intmax_t minimum, intmax_t maximum>
class has_extra_space {
private:
	using underlying_type = underlying_t<minimum, maximum>;
	static constexpr intmax_t underlying_min = std::numeric_limits<underlying_type>::min();
	static constexpr intmax_t underlying_max = std::numeric_limits<underlying_type>::max();
public:
	static constexpr bool value = underlying_min < minimum or underlying_max > maximum;
};

template<intmax_t minimum, intmax_t maximum, typename policy>
class optional<minimum, maximum, policy, typename std::enable_if<has_extra_space<minimum, maximum>::value, void>::type> {
public:
	using value_type = ranged_integer<minimum, maximum, policy>;

	constexpr optional() noexcept:
		m_value(uninitialized_value(), non_check) {
	}
	constexpr optional(none_t) noexcept:
		optional{} {
	}

	template<typename integer, enable_if_t<
		detail::is_implicitly_constructible_from<minimum, maximum, integer>()
	> = clang_dummy>
	constexpr optional(integer && other) noexcept:
		m_value(std::forward<integer>(other)) {
	}
	template<typename integer, enable_if_t<
		detail::is_explicitly_constructible_from<minimum, maximum, policy, integer>()
	> = clang_dummy>
	constexpr explicit optional(integer && other):
		m_value(std::forward<integer>(other)) {
	}

	constexpr optional(optional const &) noexcept = default;
	constexpr optional(optional &&) noexcept = default;
	template<intmax_t other_min, intmax_t other_max, typename other_policy>
	constexpr explicit optional(optional<other_min, other_max, other_policy> const & other):
		m_value(other.is_initialized() ? *other : uninitialized_value()) {
	}
	template<intmax_t other_min, intmax_t other_max, typename other_policy>
	constexpr explicit optional(optional<other_min, other_max, other_policy> && other):
		m_value(other.is_initialized() ? *std::move(other) : uninitialized_value()) {
	}

	optional & operator=(optional const &) noexcept = default;
	optional & operator=(optional &&) noexcept = default;
	template<typename integer>
	optional & operator=(integer && other) {
		*this = optional(std::forward<integer>(other));
		return *this;
	}
	
	// TODO: conform better to std::optional
	// comma operator is used to avoid binding the reference to a temporary
	constexpr value_type const & value() const {
		return is_initialized() ? m_value : (throw std::logic_error("bad optional access"), m_value);
	}
	value_type & value() {
		return is_initialized() ? m_value : (throw std::logic_error("bad optional access"), m_value);
	}
	constexpr value_type const & operator*() const {
		return value();
	}
	value_type & operator*() {
		return value();
	}
	
	constexpr value_type const * operator->() const {
		return &value();
	}
	value_type * operator->() {
		return &value();
	}
	
	constexpr value_type const & get_value_or(value_type const & default_value) const noexcept {
		return is_initialized() ? m_value : default_value;
	}
	
	constexpr explicit operator bool() const noexcept {
		return is_initialized();
	}
	constexpr bool operator!() const noexcept {
		return !is_initialized();
	}
private:
	template<intmax_t other_min, intmax_t other_max, typename other_policy, typename other_enabler>
	friend class optional;
	constexpr bool is_initialized() const noexcept {
		return m_value != uninitialized_value();
	}
	using underlying_type = typename value_type::underlying_type;
	static constexpr underlying_type uninitialized_value() {
		return static_cast<underlying_type>(
			(minimum > std::numeric_limits<underlying_type>::min()) ? minimum - 1 :
			(maximum < std::numeric_limits<underlying_type>::max()) ? maximum + 1 :
			throw std::logic_error("Attempted to use compressed optional when the default version should have been used.")
		);
	}
	value_type m_value;
};


template<intmax_t minimum, intmax_t maximum, typename policy>
class optional<minimum, maximum, policy, typename std::enable_if<!has_extra_space<minimum, maximum>::value, void>::type> {
public:
	using value_type = ranged_integer<minimum, maximum, policy>;

	constexpr optional() noexcept:
		m_value(uninitialized_value(), non_check),
		m_initialized(false) {
	}
	constexpr optional(none_t) noexcept:
		optional{} {
	}

	template<typename integer, enable_if_t<
		detail::is_implicitly_constructible_from<minimum, maximum, integer>()
	> = clang_dummy>
	constexpr optional(integer && other) noexcept:
		m_value(std::forward<integer>(other)),
		m_initialized(true) {
	}
	template<typename integer, enable_if_t<
		detail::is_explicitly_constructible_from<minimum, maximum, policy, integer>()
	> = clang_dummy>
	constexpr explicit optional(integer && other):
		m_value(std::forward<integer>(other)),
		m_initialized(true) {
	}

	constexpr optional(optional const &) noexcept = default;
	constexpr optional(optional &&) noexcept = default;
	template<intmax_t other_min, intmax_t other_max, typename other_policy>
	constexpr explicit optional(optional<other_min, other_max, other_policy> const & other):
		m_value(other.is_initialized() ? *other : uninitialized_value()),
		m_initialized(other.is_initialized()) {
	}
	template<intmax_t other_min, intmax_t other_max, typename other_policy>
	constexpr explicit optional(optional<other_min, other_max, other_policy> && other):
		m_value(other.is_initialized() ? *other : uninitialized_value()),
		m_initialized(other.m_initialized) {
	}

	optional & operator=(optional const &) noexcept = default;
	optional & operator=(optional &&) noexcept = default;
	template<typename integer>
	optional & operator=(integer && other) {
		*this = optional(std::forward<integer>(other));
		return *this;
	}

	// comma operator is used to avoid binding the reference to a temporary
	constexpr value_type const & value() const {
		return is_initialized() ? m_value : (throw std::logic_error("bad optional access"), m_value);
	}
	value_type & value() {
		return is_initialized() ? m_value : (throw std::logic_error("bad optional access"), m_value);
	}
	constexpr value_type const & operator*() const {
		return value();
	}
	value_type & operator*() {
		return value();
	}
	
	constexpr value_type const * operator->() const {
		return &value();
	}
	value_type * operator->() {
		return &value();
	}
	
	constexpr value_type const & get_value_or(value_type const & default_value) const noexcept {
		return is_initialized() ? m_value : default_value;
	}
	
	constexpr explicit operator bool() const noexcept {
		return is_initialized();
	}
	constexpr bool operator!() const noexcept {
		return !is_initialized();
	}
private:
	template<intmax_t other_min, intmax_t other_max, typename other_policy, typename other_enabler>
	friend class optional;
	constexpr bool is_initialized() const noexcept {
		return m_initialized;
	}
	using underlying_type = typename value_type::underlying_type;
	static constexpr underlying_type uninitialized_value() {
		return static_cast<underlying_type>(minimum);
	}
	value_type m_value;
	bool m_initialized;
};



template<typename T>
class optional_c;

template<intmax_t minimum, intmax_t maximum, typename policy>
class optional_c<ranged_integer<minimum, maximum, policy>> {
public:
	using type = optional<minimum, maximum, policy>;
};

}	// namespace detail

template<typename T>
using optional = typename detail::optional_c<T>::type;

template<typename T>
optional<typename std::remove_reference<T>::type> make_optional(T && value) noexcept {
	return optional<T>(std::forward<T>(value));
}

#endif	// RANGED_INTEGER_DETAIL_OPTIONAL_HPP_
