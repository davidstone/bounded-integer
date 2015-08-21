// integer type main implementation
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

#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "is_bounded_integer.hpp"
#include "noexcept.hpp"
#include "numeric_limits.hpp"
#include "overlapping_range.hpp"
#include "underlying_type.hpp"

#include "policy/null_policy.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T>
constexpr bool allow_construction_from() {
	return basic_numeric_limits<T>::is_specialized and (basic_numeric_limits<T>::is_integer or std::is_enum<std::decay_t<T>>::value);
}

template<typename T, enable_if_t<allow_construction_from<T>()> = clang_dummy>
constexpr auto is_implicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_fits_in_range<std::decay_t<T>>(minimum, maximum);
}
template<typename T, enable_if_t<!allow_construction_from<T>()> = clang_dummy>
constexpr auto is_implicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}

template<typename policy, typename T, enable_if_t<allow_construction_from<T>()> = clang_dummy>
constexpr auto is_explicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_overlaps_range<std::decay_t<T>>(minimum, maximum) or !policy::overflow_is_error;
}
template<typename policy, typename T, enable_if_t<!allow_construction_from<T>()> = clang_dummy>
constexpr auto is_explicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}

// The user can specialize basic_numeric_limits for their enumeration type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T, enable_if_t<!std::is_enum<std::decay_t<T>>::value> = clang_dummy>
constexpr decltype(auto) as_integer(T && t) noexcept {
	return static_cast<T &&>(t);
}
template<typename T, enable_if_t<std::is_enum<std::decay_t<T>>::value> = clang_dummy>
constexpr decltype(auto) as_integer(T && t) noexcept {
	using limits = basic_numeric_limits<T>;
	return integer<static_cast<std::intmax_t>(limits::min()), static_cast<std::intmax_t>(limits::max()), null_policy, storage_type::fast>(static_cast<std::underlying_type_t<std::decay_t<T>>>(t));
}

}	// namespace detail


template<typename T, enable_if_t<!is_bounded_integer<std::decay_t<T>>::value> = clang_dummy>
constexpr auto get_overflow_policy(T const &) noexcept {
	return null_policy{};
}
template<typename T, enable_if_t<is_bounded_integer<std::decay_t<T>>::value> = clang_dummy>
constexpr decltype(auto) get_overflow_policy(T && value) noexcept {
	return std::forward<T>(value).overflow_policy();
}




template<intmax_t value, typename overflow_policy = null_policy, storage_type storage = storage_type::fast>
constexpr auto constant = integer<value, value, overflow_policy, storage>(value, non_check);




template<intmax_t minimum, intmax_t maximum, typename overflow_policy_ = null_policy, storage_type storage = storage_type::fast>
struct integer : private overflow_policy_ {
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using underlying_type = std::conditional_t<storage == storage_type::fast, detail::fast_t<minimum, maximum>, detail::least_t<minimum, maximum>>;
	using overflow_policy_type = overflow_policy_;
	static_assert(detail::value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(detail::value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");

	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "underlying_type should be signed.");
	
	// May relax these restrictions in the future
	static_assert(std::is_nothrow_default_constructible<overflow_policy_type>::value, "overflow_policy must be nothrow default constructible.");
	static_assert(std::is_nothrow_move_constructible<overflow_policy_type>::value, "overflow_policy must be nothrow move constructible.");
	

	integer() noexcept = default;
	constexpr integer(integer const &) noexcept = default;
	constexpr integer(integer &&) noexcept = default;

	// Use these constructors if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename T, enable_if_t<
		detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(T && other, overflow_policy_type policy, non_check_t) noexcept:
		overflow_policy_type(std::move(policy)),
		m_value(static_cast<underlying_type>(std::forward<T>(other))) {
	}
	// std::forward is safe here because it will never actually move anything.
	// The value itself is only moved from inside of the constructor this
	// forwards to.
	template<typename T, enable_if_t<
		detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(T && other, non_check_t) noexcept:
		integer(std::forward<T>(other), static_cast<overflow_policy_type>(get_overflow_policy(std::forward<T>(other))), non_check) {
	}


	template<typename T, enable_if_t<
		detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(T && other, overflow_policy_type policy) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(policy.assignment(detail::as_integer(std::forward<T>(other)), constant<minimum>, constant<maximum>), policy, non_check)
	) {
	}


	// Intentionally implicit: this is safe because the value is in range
	template<typename T, enable_if_t<
		detail::is_implicitly_constructible_from<T>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(T && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(std::forward<T>(other), static_cast<overflow_policy_type>(get_overflow_policy(std::forward<T>(other))))
	) {
	}

	template<typename T, enable_if_t<
		!detail::is_implicitly_constructible_from<T>(minimum, maximum) and
		detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum)
	> = clang_dummy>
	constexpr explicit integer(T && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(std::forward<T>(other), static_cast<overflow_policy_type>(get_overflow_policy(std::forward<T>(other))))
	) {
	}


	template<typename Enum, enable_if_t<
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy_type, Enum>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(Enum other, overflow_policy_type policy, non_check_t) noexcept:
		integer(static_cast<std::underlying_type_t<Enum>>(other), std::move(policy), non_check) {
	}
	template<typename Enum, enable_if_t<
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy_type, Enum>(minimum, maximum)
	> = clang_dummy>
	constexpr integer(Enum other, non_check_t) noexcept:
		integer(static_cast<std::underlying_type_t<Enum>>(other), non_check) {
	}
	template<typename Enum, enable_if_t<
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy_type, Enum>(minimum, maximum)
	> = clang_dummy>
	constexpr explicit integer(Enum other, overflow_policy_type policy) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(static_cast<std::underlying_type_t<Enum>>(other), std::move(policy))
	) {
	}
	template<typename Enum, enable_if_t<
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy_type, Enum>(minimum, maximum)
	> = clang_dummy>
	constexpr explicit integer(Enum other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(static_cast<std::underlying_type_t<Enum>>(other)) 
	) {
	}


	// volatile qualified overloads do not return a reference to the object to
	// avoid any confusion over what qualifies as a read of a volatile variable.
	// gcc does not consider it a read and gives you a warning if your
	// assignment operator returns an unused reference.
	template<typename T>
	constexpr auto && unchecked_assignment(T && other) & noexcept {
		m_value = static_cast<underlying_type>(std::forward<T>(other));
		return *this;
	}
	template<typename T>
	auto unchecked_assignment(T && other) volatile & noexcept {
		m_value = static_cast<underlying_type>(std::forward<T>(other));
	}
	
	constexpr auto && operator=(integer const & other) & noexcept(noexcept(std::declval<overflow_policy_type>().assignment(other, constant<minimum>, constant<maximum>))) {
		return unchecked_assignment(overflow_policy().assignment(other, constant<minimum>, constant<maximum>));
	}
	constexpr auto && operator=(integer && other) & noexcept(noexcept(std::declval<overflow_policy_type>().assignment(std::move(other), constant<minimum>, constant<maximum>))) {
		return unchecked_assignment(overflow_policy().assignment(std::move(other), constant<minimum>, constant<maximum>));
	}
	template<typename T>
	constexpr auto && operator=(T && other) & noexcept(noexcept(std::declval<overflow_policy_type>().assignment(std::forward<T>(other), constant<minimum>, constant<maximum>))) {
		static_assert(
			detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum),
			"Value not in range."
		);
		return unchecked_assignment(overflow_policy().assignment(std::forward<T>(other), constant<minimum>, constant<maximum>));
	}
	template<typename T>
	auto operator=(T && other) volatile & noexcept(noexcept(std::declval<overflow_policy_type>().assignment(std::forward<T>(other), constant<minimum>, constant<maximum>))) {
		static_assert(
			detail::is_explicitly_constructible_from<overflow_policy_type, T>(minimum, maximum),
			"Value not in range."
		);
		unchecked_assignment(overflow_policy().assignment(std::forward<T>(other), constant<minimum>, constant<maximum>));
	}
	
	constexpr auto const & value() const noexcept {
		return m_value;
	}
	auto const & value() const volatile noexcept {
		return m_value;
	}
	constexpr auto overflow_policy() const & noexcept -> overflow_policy_type const & {
		return *this;
	}
	auto overflow_policy() const volatile & noexcept -> overflow_policy_type const volatile & {
		return *this;
	}
	constexpr auto overflow_policy() & noexcept -> overflow_policy_type & {
		return *this;
	}
	auto overflow_policy() volatile & noexcept -> overflow_policy_type volatile & {
		return *this;
	}
	constexpr auto overflow_policy() && noexcept -> overflow_policy_type && {
		return static_cast<overflow_policy_type &&>(*this);
	}
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	//
	// This must not reference the overflow policy because it is possible that
	// it has already been moved from if this is being called from the
	// constructor.
	template<typename T, enable_if_t<std::is_arithmetic<T>::value or std::is_enum<T>::value> = clang_dummy>
	constexpr explicit operator T() const noexcept {
		return static_cast<T>(m_value);
	}
	template<typename T, enable_if_t<std::is_arithmetic<T>::value or std::is_enum<T>::value> = clang_dummy>
	constexpr explicit operator T() const volatile noexcept {
		return static_cast<T>(m_value);
	}
private:
	underlying_type m_value;
};

}	// namespace bounded

