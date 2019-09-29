// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
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
concept generic_integer = basic_numeric_limits<T>::is_specialized and (basic_numeric_limits<T>::is_integer or std::is_enum_v<T>);

template<typename T, auto minimum, auto maximum, typename policy>
concept overlapping_integer = generic_integer<T> and (!policy::overflow_is_error or (compare(basic_numeric_limits<T>::min(), maximum) <= 0 and compare(minimum, basic_numeric_limits<T>::max()) <= 0));

template<typename T, auto minimum, auto maximum, typename policy>
concept bounded_by_range = overlapping_integer<T, minimum, maximum, policy> and !std::is_same_v<T, bool> and compare(minimum, basic_numeric_limits<T>::min()) <= 0 and compare(basic_numeric_limits<T>::max(), maximum) <= 0;


// Necessary for optional specialization
template<typename T>
concept has_extra_space =
	basic_numeric_limits<T>::min() != basic_numeric_limits<T>::max() and
	(
	basic_numeric_limits<typename T::underlying_type>::min() < basic_numeric_limits<T>::min() or
		basic_numeric_limits<T>::max() < basic_numeric_limits<typename T::underlying_type>::max()
	);


template<typename Integer>
constexpr auto as_builtin_integer(Integer const x) {
	if constexpr (detail_builtin_integer<Integer>) {
		return x;
	} else {
		static_assert(bounded_integer<Integer>);
		return x.value();
	}
}

// The user can specialize basic_numeric_limits for their enumeration type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T>
constexpr decltype(auto) as_integer(T const & value) {
	if constexpr (std::is_enum_v<T>) {
		using limits = basic_numeric_limits<T>;
		using result_type = integer<
			as_builtin_integer(limits::min()),
			as_builtin_integer(limits::max()),
			null_policy
		>;
		return result_type(static_cast<std::underlying_type_t<T>>(value));
	} else if constexpr (std::is_same_v<T, bool>) {
		return integer<0, 1, null_policy>(value, non_check);
	} else {
		return value;
	}
}


template<typename T>
struct value_wrapper {
	using underlying_type = T;
	value_wrapper() = default;
	constexpr explicit value_wrapper(underlying_type const value_):
		m_value(value_)
	{
	}
	constexpr auto value() const {
		return m_value;
	}
	constexpr auto value() const volatile {
		return m_value;
	}
	constexpr auto assign(underlying_type other) {
		m_value = other;
	}
	auto assign(underlying_type other) volatile {
		m_value = other;
	}
private:
	underlying_type m_value;
};

template<auto value_>
struct constant_wrapper {
	using underlying_type = decltype(value_);
	constexpr constant_wrapper() = default;
	constexpr explicit constant_wrapper(underlying_type const &) {
	}
	static constexpr auto value() {
		return value_;
	}
	static constexpr auto assign(underlying_type const &) {
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


// TODO: Use terse concepts syntax on most of these functions after fix for
// https://github.com/saarraz/clang-concepts-monorepo/issues/20 and
// https://github.com/saarraz/clang-concepts-monorepo/issues/21

template<auto minimum, auto maximum, typename overflow_policy_ = null_policy>
struct integer : private detail::base<minimum, maximum> {
private:
	static_assert(std::is_same_v<decltype(minimum), std::remove_const_t<decltype(detail::normalize<minimum>)>>);
	static_assert(std::is_same_v<decltype(maximum), std::remove_const_t<decltype(detail::normalize<maximum>)>>);
	using base = detail::base<minimum, maximum>;
public:
	static_assert(compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = typename base::underlying_type;
	using overflow_policy = overflow_policy_;
	static_assert(detail::value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(detail::value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");
	
	static constexpr auto min() {
		return constant<minimum>;
	}
	static constexpr auto max() {
		return constant<maximum>;
	}

private:
	template<typename T>
	static constexpr decltype(auto) apply_overflow_policy(T const & value) {
		return overflow_policy{}.assignment(value, min(), max());
	}
public:
	
	using base::value;
	
	integer() = default;
	constexpr integer(integer const &) = default;
	constexpr integer(integer &&) = default;

	// All constructors not taking a non_check_t argument accept an
	// overflow_policy, which they default and ignore. This is solely to make
	// the class work better with deduction guides.

	// Use non_check_t constructors if you know by means that cannot be
	// determined by the type system that the value fits in the range.

	template<typename T> requires detail::overlapping_integer<T, minimum, maximum, overflow_policy>
	constexpr integer(T const & other, non_check_t):
		base(static_cast<underlying_type>(other)) {
	}

	template<typename T> requires detail::bounded_by_range<T, minimum, maximum, overflow_policy>
	constexpr integer(T const other, overflow_policy = overflow_policy{}):
		integer(other, non_check)
	{
	}

	template<typename T> requires detail::overlapping_integer<T, minimum, maximum, overflow_policy>
	constexpr explicit integer(T const & other, overflow_policy = overflow_policy{}):
		integer(apply_overflow_policy(detail::as_integer(other)), non_check)
	{
	}

	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::overlapping_integer<Enum, minimum, maximum, overflow_policy>
	)
	constexpr integer(Enum other, non_check_t):
		integer(static_cast<std::underlying_type_t<Enum>>(other), non_check) {
	}
	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::overlapping_integer<Enum, minimum, maximum, overflow_policy>
	)
	constexpr explicit integer(Enum other, overflow_policy = overflow_policy{}):
		integer(static_cast<std::underlying_type_t<Enum>>(other))
	{
	}


	template<typename T>
	constexpr auto && unchecked_assignment(T const & other) & {
		base::assign(static_cast<underlying_type>(other));
		return *this;
	}
	template<typename T>
	auto unchecked_assignment(T const & other) volatile & {
		base::assign(static_cast<underlying_type>(other));
	}
	
	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	template<typename T> requires detail::overlapping_integer<T, minimum, maximum, overflow_policy>
	constexpr auto && operator=(T const & other) & {
		return unchecked_assignment(apply_overflow_policy(other));
	}
	template<typename T> requires detail::overlapping_integer<T, minimum, maximum, overflow_policy>
	auto operator=(T const & other) volatile & {
		unchecked_assignment(apply_overflow_policy(other));
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (detail_builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}
	template<typename T> requires (detail_builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const volatile {
		return static_cast<T>(value());
	}
	
	// Allow a compressed optional representation
	template<typename Tag> requires (std::is_same_v<Tag, optional_tag> and detail::has_extra_space<integer>)
	constexpr explicit integer(Tag):
		base(uninitialized_value()) {
	}

	template<typename... Args> requires(
		detail::has_extra_space<integer> and
		std::is_constructible_v<integer, Args...>
	)
	constexpr auto initialize(optional_tag, Args... args) {
		return *this = integer(args...);
	}

	template<typename Tag> requires (
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer>
	)
	constexpr auto uninitialize(Tag) {
		base::assign(uninitialized_value());
	}
	template<typename Tag> requires (
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer>
	)
	constexpr auto is_initialized(Tag) const {
		return base::value() != uninitialized_value();
	}
private:
	static constexpr auto uninitialized_value() {
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
constexpr auto deduced_min() {
	return basic_numeric_limits<T>::min();
}
template<typename T> requires basic_numeric_limits<T>::is_specialized
constexpr auto deduced_max() {
	return basic_numeric_limits<T>::max();
}

template<typename T> requires (not basic_numeric_limits<T>::is_specialized)
constexpr auto deduced_min() {
	return basic_numeric_limits<std::underlying_type_t<T>>::min();
}
template<typename T> requires (not basic_numeric_limits<T>::is_specialized)
constexpr auto deduced_max() {
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
