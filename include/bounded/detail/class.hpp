// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/is_poisoned.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/requires.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <bounded/detail/optional/forward_declaration.hpp>

#include <bounded/detail/policy/null_policy.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace bounded {

template<intmax_t minimum, intmax_t maximum, typename policy = null_policy, storage_type storage = storage_type::fast, bool poisoned = false>
using integer = detail::basic_integer<minimum, maximum, policy, storage, poisoned>;

namespace detail {

template<typename T>
constexpr auto allow_construction_from = basic_numeric_limits<T>::is_specialized and (basic_numeric_limits<T>::is_integer or std::is_enum<std::decay_t<T>>::value);

template<typename T>
constexpr auto is_implicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	if constexpr (allow_construction_from<T> and !std::is_same<std::decay_t<T>, bool>{}) {
		return type_fits_in_range<std::decay_t<T>>(minimum, maximum);
	} else {
		static_cast<void>(minimum);
		static_cast<void>(maximum);
		return std::false_type{};
	}
}

template<typename policy, typename T>
constexpr auto is_explicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	if constexpr (allow_construction_from<T>) {
		return type_overlaps_range<std::decay_t<T>>(minimum, maximum) or !policy::overflow_is_error;
	} else {
		static_cast<void>(minimum);
		static_cast<void>(maximum);
		return std::false_type{};
	}
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
template<typename T>
constexpr decltype(auto) as_integer(T const & t) noexcept {
	if constexpr (std::is_enum<std::decay_t<T>>::value) {
		using limits = basic_numeric_limits<T>;
		using result_type = basic_integer<
			static_cast<std::intmax_t>(limits::min()),
			static_cast<std::intmax_t>(limits::max()),
			null_policy,
			storage_type::fast,
			false
		>;
		return result_type(static_cast<std::underlying_type_t<std::decay_t<T>>>(t));
	} else {
		return t;
	}
}

}	// namespace detail


// poisoned is useful for overloading and getting all constants
template<intmax_t value, typename overflow_policy = null_policy, storage_type storage = storage_type::fast, bool poisoned = false>
using constant_t = detail::basic_integer<value, value, overflow_policy, storage, poisoned>;

template<intmax_t value, typename overflow_policy = null_policy, storage_type storage = storage_type::fast>
constexpr auto constant = constant_t<value, overflow_policy, storage>(value, non_check);


namespace detail {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy_ = null_policy, storage_type storage = storage_type::fast, bool poisoned_ = false>
struct basic_integer {
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using underlying_type = std::conditional_t<storage == storage_type::fast, fast_t<minimum, maximum>, least_t<minimum, maximum>>;
	using overflow_policy = overflow_policy_;
	static_assert(value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");
	
	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "underlying_type should be signed.");
	
	// May relax these restrictions in the future
	static_assert(std::is_nothrow_default_constructible<overflow_policy>::value, "overflow_policy must be nothrow default constructible.");
	
	static constexpr auto min() noexcept {
		return constant<minimum>;
	}
	static constexpr auto max() noexcept {
		return constant<maximum>;
	}
	
	template<typename T>
	static constexpr decltype(auto) apply_overflow_policy(T const & value) BOUNDED_NOEXCEPT(
		overflow_policy{}.assignment(value, min(), max())
	)
	
	basic_integer() noexcept = default;
	constexpr basic_integer(basic_integer const &) noexcept = default;
	constexpr basic_integer(basic_integer &&) noexcept = default;

	// Use these constructors if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename T, BOUNDED_REQUIRES(
		is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum)
	)>
	constexpr basic_integer(T const & other, non_check_t) noexcept:
		m_value(static_cast<underlying_type>(other)) {
	}


	// Intentionally implicit: this is safe because the value is in range
	template<typename T, BOUNDED_REQUIRES(
		is_implicitly_constructible_from<T const &>(minimum, maximum)
	)>
	constexpr basic_integer(T const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_integer(other, non_check)
	) {
	}

	template<typename T, BOUNDED_REQUIRES(
		!is_implicitly_constructible_from<T const &>(minimum, maximum) and
		is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum) and
		!is_poisoned<T>
	)>
	constexpr explicit basic_integer(T const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_integer(apply_overflow_policy(detail::as_integer(other)), non_check)
	) {
	}

	template<typename T, BOUNDED_REQUIRES(
		!is_implicitly_constructible_from<T const &>(minimum, maximum) and
		is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum) and
		is_poisoned<T>
	)>
	constexpr basic_integer(T const & other) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_integer(apply_overflow_policy(detail::as_integer(other)), non_check)
	) {
	}


	template<typename Enum, BOUNDED_REQUIRES(
		std::is_enum<Enum>::value and !is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)>
	constexpr basic_integer(Enum other, non_check_t) noexcept:
		basic_integer(static_cast<std::underlying_type_t<Enum>>(other), non_check) {
	}
	template<typename Enum, BOUNDED_REQUIRES(
		std::is_enum<Enum>::value and !is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)>
	constexpr explicit basic_integer(Enum other) BOUNDED_NOEXCEPT_INITIALIZATION(
		basic_integer(static_cast<std::underlying_type_t<Enum>>(other)) 
	) {
	}


	// volatile qualified overloads do not return a reference to the object to
	// avoid any confusion over what qualifies as a read of a volatile variable.
	// gcc does not consider it a read and gives you a warning if your
	// assignment operator returns an unused reference.
	template<typename T>
	constexpr auto && unchecked_assignment(T const & other) & noexcept {
		m_value = static_cast<underlying_type>(other);
		return *this;
	}
	template<typename T>
	auto unchecked_assignment(T const & other) volatile & noexcept {
		m_value = static_cast<underlying_type>(other);
	}
	
	constexpr auto operator=(basic_integer const & other) & noexcept -> basic_integer & = default;
	constexpr auto operator=(basic_integer && other) & noexcept -> basic_integer & = default;

	template<typename T>
	constexpr auto && operator=(T const & other) & noexcept(noexcept(apply_overflow_policy(other))) {
		static_assert(
			is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum),
			"Value not in range."
		);
		return unchecked_assignment(apply_overflow_policy(other));
	}
	template<typename T>
	auto operator=(T const & other) volatile & noexcept(noexcept(apply_overflow_policy(other))) {
		static_assert(
			is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum),
			"Value not in range."
		);
		unchecked_assignment(apply_overflow_policy(other));
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
	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and has_extra_space<basic_integer>)>
	constexpr explicit basic_integer(Tag) noexcept:
		m_value(uninitialized_value()) {
	}

	// Cannot use BOUNDED_NOEXCEPT_VOID because of gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869
	template<typename Tag, typename... Args, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and has_extra_space<basic_integer> and std::is_constructible<basic_integer, Args...>{})>
	constexpr auto initialize(Tag, Args && ... args) noexcept(std::is_nothrow_constructible<basic_integer, Args...>{}) {
		return *this = basic_integer(std::forward<Args>(args)...);
	}

	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and has_extra_space<basic_integer>)>
	constexpr auto uninitialize(Tag) noexcept {
		m_value = uninitialized_value();
	}
	template<typename Tag, BOUNDED_REQUIRES(std::is_same<Tag, optional_tag>::value and has_extra_space<basic_integer>)>
	constexpr auto is_initialized(Tag) const noexcept {
		return m_value != uninitialized_value();
	}
private:
	static constexpr auto uninitialized_value() noexcept {
		return static_cast<underlying_type>(minimum > std::numeric_limits<underlying_type>::min() ? minimum - 1 : maximum + 1);
	}

	underlying_type m_value;
};

}	// namespace detail
}	// namespace bounded