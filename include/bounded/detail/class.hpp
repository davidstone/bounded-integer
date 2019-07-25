// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <bounded/detail/optional/forward_declaration.hpp>

#include <bounded/detail/policy/null_policy.hpp>

#include <limits>
#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T>
inline constexpr auto allow_construction_from = basic_numeric_limits<T>::is_specialized and (basic_numeric_limits<T>::is_integer or std::is_enum<std::decay_t<T>>{});

template<typename T, typename Minimum, typename Maximum>
constexpr auto is_implicitly_constructible_from(Minimum const minimum [[maybe_unused]], Maximum const maximum [[maybe_unused]]) noexcept {
	if constexpr (allow_construction_from<T> and !std::is_same<std::decay_t<T>, bool>{}) {
		return type_fits_in_range<std::decay_t<T>>(minimum, maximum);
	} else {
		return false;
	}
}

template<typename policy, typename T, typename Minimum, typename Maximum>
constexpr auto is_explicitly_constructible_from(Minimum const minimum [[maybe_unused]], Maximum const maximum [[maybe_unused]]) noexcept {
	if constexpr (allow_construction_from<T>) {
		return type_overlaps_range<std::decay_t<T>>(minimum, maximum) or !policy::overflow_is_error;
	} else {
		return false;
	}
}


// Necessary for optional specialization
template<typename T>
constexpr auto has_extra_space =
	basic_numeric_limits<typename T::underlying_type>::min() < basic_numeric_limits<T>::min() or
	basic_numeric_limits<T>::max() < basic_numeric_limits<typename T::underlying_type>::max();


template<typename Integer>
constexpr auto as_builtin_integer(Integer const x) noexcept {
	if constexpr (detail::is_builtin_integer<Integer>) {
		return x;
	} else {
		static_assert(is_bounded_integer<Integer>);
		return x.value();
	}
}

// The user can specialize basic_numeric_limits for their enumeration type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T>
constexpr decltype(auto) as_integer(T const & t) noexcept {
	if constexpr (std::is_enum<std::decay_t<T>>{}) {
		using limits = basic_numeric_limits<T>;
		using result_type = integer<
			as_builtin_integer(limits::min()),
			as_builtin_integer(limits::max()),
			null_policy
		>;
		return result_type(static_cast<std::underlying_type_t<std::decay_t<T>>>(t));
	} else {
		return t;
	}
}


template<typename T>
struct value_wrapper {
	using underlying_type = T;
	value_wrapper() noexcept = default;
	constexpr explicit value_wrapper(underlying_type const value_) noexcept:
		m_value(value_)
	{
	}
	constexpr auto value() const noexcept {
		return m_value;
	}
	constexpr auto value() const volatile noexcept {
		return m_value;
	}
	constexpr auto assign(underlying_type other) noexcept {
		m_value = other;
	}
	auto assign(underlying_type other) volatile noexcept {
		m_value = other;
	}
private:
	underlying_type m_value;
};

template<auto value_>
struct constant_wrapper {
	using underlying_type = decltype(value_);
	constexpr constant_wrapper() noexcept = default;
	constexpr explicit constant_wrapper(underlying_type const &) noexcept {
	}
	static constexpr auto value() noexcept {
		return value_;
	}
	static constexpr auto assign(underlying_type const &) noexcept {
	}
};

template<auto minimum, auto maximum>
using base = std::conditional_t<minimum == maximum,
	constant_wrapper<static_cast<underlying_type_t<minimum, maximum>>(minimum)>,
	value_wrapper<underlying_type_t<minimum, maximum>>
>;

}	// namespace detail


template<auto value, typename overflow_policy = null_policy>
using constant_t = integer<value, value, overflow_policy>;

template<auto value, typename overflow_policy = null_policy>
inline constexpr auto constant = constant_t<detail::normalize<value>, overflow_policy>{};


template<auto minimum, auto maximum, typename overflow_policy_ = null_policy>
struct integer : private detail::base<minimum, maximum> {
private:
	static_assert(std::is_same_v<std::decay_t<decltype(minimum)>, std::decay_t<decltype(detail::normalize<minimum>)>>);
	static_assert(std::is_same_v< std::decay_t<decltype(maximum)>, std::decay_t<decltype(detail::normalize<maximum>)>>);
	using base = detail::base<minimum, maximum>;
public:
	static_assert(compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = typename base::underlying_type;
	using overflow_policy = overflow_policy_;
	static_assert(detail::value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(detail::value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");
	
	// May relax these restrictions in the future
	static_assert(std::is_nothrow_default_constructible_v<overflow_policy>, "overflow_policy must be nothrow default constructible.");
	
	static constexpr auto min() noexcept {
		return constant<minimum>;
	}
	static constexpr auto max() noexcept {
		return constant<maximum>;
	}

private:
	template<typename T>
	static constexpr decltype(auto) apply_overflow_policy(T const & value) BOUNDED_NOEXCEPT(
		overflow_policy{}.assignment(value, min(), max())
	)
public:
	
	using base::value;
	
	integer() noexcept = default;
	constexpr integer(integer const &) noexcept = default;
	constexpr integer(integer &&) noexcept = default;

	// All constructors not taking a non_check_t argument accept an
	// overflow_policy, which they default and ignore. This is solely to make
	// the class work better with deduction guides.

	// Use non_check_t constructors if you know by means that cannot be
	// determined by the type system that the value fits in the range.
	template<typename T> requires(
		detail::is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum)
	)
	constexpr integer(T const & other, non_check_t) noexcept:
		base(static_cast<underlying_type>(other)) {
	}


	// Intentionally implicit: this is safe because the value is in range
	template<typename T> requires(
		detail::is_implicitly_constructible_from<T const &>(minimum, maximum)
	)
	constexpr integer(T const & other, overflow_policy = overflow_policy{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(other, non_check)
	) {
	}

	template<typename T> requires(
		!detail::is_implicitly_constructible_from<T const &>(minimum, maximum) and
		detail::is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum) and
		!std::is_same_v<T, bool>
	)
	constexpr explicit integer(T const & other, overflow_policy = overflow_policy{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(apply_overflow_policy(detail::as_integer(other)), non_check)
	) {
	}

	template<typename T> requires(
		detail::is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum) and
		std::is_same_v<T, bool>
	)
	constexpr explicit integer(T const & other, overflow_policy = overflow_policy{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(apply_overflow_policy(integer<0, 1>(other, non_check)), non_check)
	) {
	}

	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)
	constexpr integer(Enum other, non_check_t) noexcept:
		integer(static_cast<std::underlying_type_t<Enum>>(other), non_check) {
	}
	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::is_explicitly_constructible_from<overflow_policy, Enum>(minimum, maximum)
	)
	constexpr explicit integer(Enum other, overflow_policy = overflow_policy{}) BOUNDED_NOEXCEPT_INITIALIZATION(
		integer(static_cast<std::underlying_type_t<Enum>>(other)) 
	) {
	}


	// volatile qualified overloads do not return a reference to the object to
	// avoid any confusion over what qualifies as a read of a volatile variable.
	// gcc does not consider it a read and gives you a warning if your
	// assignment operator returns an unused reference.
	template<typename T>
	constexpr auto && unchecked_assignment(T const & other) & noexcept {
		base::assign(static_cast<underlying_type>(other));
		return *this;
	}
	template<typename T>
	auto unchecked_assignment(T const & other) volatile & noexcept {
		base::assign(static_cast<underlying_type>(other));
	}
	
	constexpr auto operator=(integer const & other) & noexcept -> integer & = default;
	constexpr auto operator=(integer && other) & noexcept -> integer & = default;

	template<typename T> requires(
		detail::is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum)
	)
	constexpr auto && operator=(T const & other) & noexcept(noexcept(apply_overflow_policy(other))) {
		return unchecked_assignment(apply_overflow_policy(other));
	}
	template<typename T> requires(
		detail::is_explicitly_constructible_from<overflow_policy, T const &>(minimum, maximum)
	)
	auto operator=(T const & other) volatile & noexcept(noexcept(apply_overflow_policy(other))) {
		unchecked_assignment(apply_overflow_policy(other));
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (detail::is_builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const noexcept {
		return static_cast<T>(value());
	}
	template<typename T> requires (detail::is_builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const volatile noexcept {
		return static_cast<T>(value());
	}
	
	// Allow a compressed optional representation
	template<typename Tag> requires (std::is_same_v<Tag, optional_tag> and detail::has_extra_space<integer>)
	constexpr explicit integer(Tag) noexcept:
		base(uninitialized_value()) {
	}

	// Cannot use BOUNDED_NOEXCEPT_VOID because of gcc bug
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869
	template<typename Tag, typename... Args> requires(
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer> and
		std::is_constructible_v<integer, Args...>
	)
	constexpr auto initialize(Tag, Args... args) noexcept(std::is_nothrow_constructible_v<integer, Args...>) {
		return *this = integer(args...);
	}

	template<typename Tag> requires (
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer>
	)
	constexpr auto uninitialize(Tag) noexcept {
		base::assign(uninitialized_value());
	}
	template<typename Tag> requires (
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer>
	)
	constexpr auto is_initialized(Tag) const noexcept {
		return base::value() != uninitialized_value();
	}
private:
	static constexpr auto uninitialized_value() noexcept {
		return minimum > basic_numeric_limits<underlying_type>::min() ?
			static_cast<underlying_type>(minimum - 1) : static_cast<underlying_type>(maximum + 1);
	}
};


namespace detail {

template<typename T>
struct equivalent_overflow_policy_c {
	using type = null_policy;
};
template<auto minimum, auto maximum, typename overflow_policy>
struct equivalent_overflow_policy_c<integer<minimum, maximum, overflow_policy>> {
	using type = overflow_policy;
};

template<typename T> requires basic_numeric_limits<T>::is_specialized
constexpr auto deduced_min() noexcept {
	return basic_numeric_limits<T>::min();
}
template<typename T> requires basic_numeric_limits<T>::is_specialized
constexpr auto deduced_max() noexcept {
	return basic_numeric_limits<T>::max();
}

template<typename T> requires (not basic_numeric_limits<T>::is_specialized)
constexpr auto deduced_min() noexcept {
	return basic_numeric_limits<std::underlying_type_t<T>>::min();
}
template<typename T> requires (not basic_numeric_limits<T>::is_specialized)
constexpr auto deduced_max() noexcept {
	return basic_numeric_limits<std::underlying_type_t<T>>::max();
}

}	// namespace detail

template<typename T, typename overflow_policy = typename detail::equivalent_overflow_policy_c<std::decay_t<T>>::type>
integer(T const & value, overflow_policy = overflow_policy{}) -> integer<
	detail::normalize<detail::deduced_min<T>()>,
	detail::normalize<detail::deduced_max<T>()>,
	overflow_policy
>;

#if 0
template<typename T>
integer(T const & value, non_check_t) = delete;

template<typename T, typename overflow_policy>
integer(T const & value, overflow_policy, non_check_t) = delete;
#endif

}	// namespace bounded
