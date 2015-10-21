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

#include "forward_declaration.hpp"
#include "is_bounded_integer.hpp"
#include "is_poisoned.hpp"
#include "noexcept.hpp"
#include "numeric_limits.hpp"
#include "overlapping_range.hpp"
#include "requires.hpp"
#include "underlying_type.hpp"

#include "optional/forward_declaration.hpp"

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

template<typename T, BOUNDED_REQUIRES(allow_construction_from<T>())>
constexpr auto is_implicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_fits_in_range<std::decay_t<T>>(minimum, maximum);
}
template<typename T, BOUNDED_REQUIRES(!allow_construction_from<T>())>
constexpr auto is_implicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}

template<typename policy, typename T, BOUNDED_REQUIRES(allow_construction_from<T>())>
constexpr auto is_explicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_overlaps_range<std::decay_t<T>>(minimum, maximum) or !policy::overflow_is_error;
}
template<typename policy, typename T, BOUNDED_REQUIRES(!allow_construction_from<T>())>
constexpr auto is_explicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}


// Necessary for optional specialization
template<typename T>
constexpr auto underlying_min = basic_numeric_limits<typename T::underlying_type>::min();
template<typename T>
constexpr auto underlying_max = basic_numeric_limits<typename T::underlying_type>::max();

template<typename T>
constexpr auto has_extra_space = underlying_min<T> < basic_numeric_limits<T>::min() or basic_numeric_limits<T>::max() < underlying_max<T>;


// The user can specialize basic_numeric_limits for their enumeration type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T, BOUNDED_REQUIRES(!std::is_enum<std::decay_t<T>>::value)>
constexpr decltype(auto) as_integer(T && t) noexcept {
	return static_cast<T &&>(t);
}
template<typename T, BOUNDED_REQUIRES(std::is_enum<std::decay_t<T>>::value)>
constexpr decltype(auto) as_integer(T && t) noexcept {
	using limits = basic_numeric_limits<T>;
	using result_type = integer<
		static_cast<std::intmax_t>(limits::min()),
		static_cast<std::intmax_t>(limits::max()),
		null_policy,
		storage_type::fast,
		false
	>;
	return result_type(static_cast<std::underlying_type_t<std::decay_t<T>>>(t));
}

}	// namespace detail


template<intmax_t value, typename overflow_policy = null_policy, storage_type storage = storage_type::fast>
constexpr auto constant = integer<value, value, overflow_policy, storage, false>(value, non_check);




template<intmax_t minimum, intmax_t maximum, typename overflow_policy_ = null_policy, storage_type storage = storage_type::fast, bool poisoned_ = false>
struct integer {
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using underlying_type = std::conditional_t<storage == storage_type::fast, detail::fast_t<minimum, maximum>, detail::least_t<minimum, maximum>>;
	using overflow_policy = overflow_policy_;
	static_assert(detail::value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(detail::value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");
	
	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "underlying_type should be signed.");
	
	// May relax these restrictions in the future
	static_assert(std::is_nothrow_default_constructible<overflow_policy>::value, "overflow_policy must be nothrow default constructible.");
	
	template<typename T>
	static constexpr decltype(auto) apply_overflow_policy(T && value) BOUNDED_NOEXCEPT(
		overflow_policy{}.assignment(std::forward<T>(value), constant<minimum>, constant<maximum>)
	)
	
	integer() noexcept = default;
	constexpr integer(integer const &) noexcept = default;
	constexpr integer(integer &&) noexcept = default;

	// Use these constructors if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename T, BOUNDED_REQUIRES(
		detail::is_explicitly_constructible_from<overflow_policy, T>(minimum, maximum)
	)>
	constexpr integer(T && other, non_check_t) noexcept:
		m_value(static_cast<underlying_type>(std::forward<T>(other))) {
	}


	// Intentionally implicit: this is safe because the value is in range
	template<typename T, BOUNDED_REQUIRES(
		detail::is_implicitly_constructible_from<T>(minimum, maximum)
	)>
	constexpr integer(T && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(std::forward<T>(other), non_check)
	) {
	}

	template<typename T, BOUNDED_REQUIRES(
		!detail::is_implicitly_constructible_from<T>(minimum, maximum) and
		detail::is_explicitly_constructible_from<overflow_policy, T>(minimum, maximum) and
		!detail::is_poisoned<T>
	)>
	constexpr explicit integer(T && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(apply_overflow_policy(detail::as_integer(std::forward<T>(other))), non_check)
	) {
	}

	template<typename T, BOUNDED_REQUIRES(
		!detail::is_implicitly_constructible_from<T>(minimum, maximum) and
		detail::is_explicitly_constructible_from<overflow_policy, T>(minimum, maximum) and
		detail::is_poisoned<T>
	)>
	constexpr integer(T && other) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(apply_overflow_policy(detail::as_integer(std::forward<T>(other))), non_check)
	) {
	}


	template<typename Enum, BOUNDED_REQUIRES(
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)>
	constexpr integer(Enum other, non_check_t) noexcept:
		integer(static_cast<std::underlying_type_t<Enum>>(other), non_check) {
	}
	template<typename Enum, BOUNDED_REQUIRES(
		std::is_enum<Enum>::value and !detail::is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)>
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
	
	constexpr auto operator=(integer const & other) & noexcept -> integer & = default;
	constexpr auto operator=(integer && other) & noexcept -> integer & = default;

	template<typename T>
	constexpr auto && operator=(T && other) & noexcept(noexcept(apply_overflow_policy(std::forward<T>(other)))) {
		static_assert(
			detail::is_explicitly_constructible_from<overflow_policy, T>(minimum, maximum),
			"Value not in range."
		);
		return unchecked_assignment(apply_overflow_policy(std::forward<T>(other)));
	}
	template<typename T>
	auto operator=(T && other) volatile & noexcept(noexcept(apply_overflow_policy(std::forward<T>(other)))) {
		static_assert(
			detail::is_explicitly_constructible_from<overflow_policy, T>(minimum, maximum),
			"Value not in range."
		);
		unchecked_assignment(apply_overflow_policy(std::forward<T>(other)));
	}
	
	constexpr auto const & value() const noexcept {
		return m_value;
	}
	auto const & value() const volatile noexcept {
		return m_value;
	}

	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	//
	// This must not reference the overflow policy because it is possible that
	// it has already been moved from if this is being called from the
	// constructor.
	template<typename T, BOUNDED_REQUIRES(std::is_arithmetic<T>::value or std::is_enum<T>::value)>
	constexpr explicit operator T() const noexcept {
		return static_cast<T>(m_value);
	}
	template<typename T, BOUNDED_REQUIRES(std::is_arithmetic<T>::value or std::is_enum<T>::value)>
	constexpr explicit operator T() const volatile noexcept {
		return static_cast<T>(m_value);
	}
	
	
	// Allow a compressed optional representation
	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and detail::has_extra_space<integer>)>
	constexpr explicit integer(Tag) noexcept:
		m_value(uninitialized_value()) {
	}

	template<typename Tag, typename... Args, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and detail::has_extra_space<integer>)>
	constexpr auto initialize(Tag, Args && ... args) noexcept(std::is_nothrow_constructible<integer, Args && ...>::value) {
		*this = integer(std::forward<Args>(args)...);
	}

	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and detail::has_extra_space<integer>)>
	constexpr auto uninitialize(Tag) noexcept {
		m_value = uninitialized_value();
	}
	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and detail::has_extra_space<integer>)>
	constexpr auto is_initialized(Tag) const noexcept {
		return m_value != uninitialized_value();
	}
private:
	static constexpr auto uninitialized_value() noexcept {
		return static_cast<underlying_type>(minimum > std::numeric_limits<underlying_type>::min() ? minimum - 1 : maximum + 1);
	}

	underlying_type m_value;
};

}	// namespace bounded

