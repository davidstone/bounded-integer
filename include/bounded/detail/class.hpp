// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/assume.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/min_max_value.hpp>
#include <bounded/detail/normalize.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <bounded/detail/optional/forward_declaration.hpp>

#include <bounded/detail/policy/null_policy.hpp>

#include <limits>
#include <type_traits>
#include <utility>

namespace bounded {

template<typename T>
inline constexpr auto is_integer = detail_builtin_integer<T> or bounded_integer<T>;

template<typename T, T value>
inline constexpr auto is_integer<std::integral_constant<T, value>> = is_integer<T>;

template<typename T>
concept integral = is_integer<T>;


namespace detail {

template<typename T, auto minimum, auto maximum, typename policy>
concept overlapping_integer =
	(is_integer<T> or std::is_enum_v<T> or std::is_same_v<T, bool>) and
	(!policy::overflow_is_error or (compare(builtin_min_value<T>, maximum) <= 0 and compare(minimum, builtin_max_value<T>) <= 0));

template<typename T, auto minimum, auto maximum, typename policy>
concept bounded_by_range = overlapping_integer<T, minimum, maximum, policy> and !std::is_same_v<T, bool> and !std::is_enum_v<T> and compare(minimum, builtin_min_value<T>) <= 0 and compare(builtin_max_value<T>, maximum) <= 0;


// Necessary for optional specialization
template<typename T>
concept has_extra_space =
	builtin_min_value<T> != builtin_max_value<T> and
	(
		min_value<typename T::underlying_type> < builtin_min_value<T> or
		builtin_max_value<T> < max_value<typename T::underlying_type>
	);

// The user can specialize min_value and max_value for their enum type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T>
constexpr decltype(auto) as_integer(T const & value) {
	if constexpr (std::is_enum_v<T>) {
		using result_type = integer<
			normalize<builtin_min_value<T>>,
			normalize<builtin_max_value<T>>,
			null_policy
		>;
		return result_type(static_cast<std::underlying_type_t<T>>(value));
	} else if constexpr (std::is_same_v<T, bool>) {
		return integer<0, 1, null_policy>(value, non_check);
	} else {
		return value;
	}
}


struct empty {
	constexpr empty() = default;
	constexpr explicit empty(auto) {
	}
};

}	// namespace detail


template<auto value, typename overflow_policy = null_policy>
using constant_t = integer<value, value, overflow_policy>;

template<auto value, typename overflow_policy = null_policy>
inline constexpr auto constant = constant_t<detail::normalize<value>, overflow_policy>{};


// TODO: Use terse concepts syntax on most of these functions after fix for
// https://github.com/saarraz/clang-concepts-monorepo/issues/20 and
// https://github.com/saarraz/clang-concepts-monorepo/issues/21

template<auto minimum, auto maximum, typename overflow_policy_ = null_policy>
struct integer {
	static_assert(std::is_same_v<decltype(minimum), std::remove_const_t<decltype(detail::normalize<minimum>)>>);
	static_assert(std::is_same_v<decltype(maximum), std::remove_const_t<decltype(detail::normalize<maximum>)>>);
	static_assert(compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = detail::underlying_type_t<minimum, maximum>;
	using overflow_policy = overflow_policy_;
	
	static constexpr auto min() {
		return constant<minimum>;
	}
	static constexpr auto max() {
		return constant<maximum>;
	}

	constexpr auto value() const requires(minimum != maximum) {
		BOUNDED_ASSUME(minimum <= m_value and m_value <= maximum);
		return m_value;
	}
	static constexpr auto value() requires(minimum == maximum) {
		return static_cast<underlying_type>(minimum);
	}
	
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
		m_value(static_cast<underlying_type>(other)) {
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
		if constexpr (minimum != maximum) {
			m_value = static_cast<underlying_type>(other);
		}
		return *this;
	}
	
	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	template<typename T> requires detail::overlapping_integer<T, minimum, maximum, overflow_policy>
	constexpr auto && operator=(T const & other) & {
		return unchecked_assignment(apply_overflow_policy(other));
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (detail_builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}
	
	// Allow a compressed optional representation
	template<typename Tag> requires (std::is_same_v<Tag, optional_tag> and detail::has_extra_space<integer>)
	constexpr explicit integer(Tag):
		m_value(uninitialized_value()) {
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
		unchecked_assignment(uninitialized_value());
	}
	template<typename Tag> requires (
		std::is_same_v<Tag, optional_tag> and
		detail::has_extra_space<integer>
	)
	constexpr auto is_initialized(Tag) const {
		return m_value != uninitialized_value();
	}
private:
	static constexpr auto uninitialized_value() {
		return minimum > min_value<underlying_type> ?
			static_cast<underlying_type>(minimum - 1) : static_cast<underlying_type>(maximum + 1);
	}

	template<typename T>
	static constexpr decltype(auto) apply_overflow_policy(T const & value) {
		return overflow_policy{}.assignment(value, min(), max());
	}

	using storage_type = std::conditional_t<minimum == maximum, detail::empty, underlying_type>;
	[[no_unique_address]] storage_type m_value;
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

template<typename T>
constexpr auto deduced_min() {
	return builtin_min_value<T>;
}
template<typename T>
constexpr auto deduced_max() {
	return builtin_max_value<T>;
}

template<typename T> requires std::is_same_v<decltype(min_value<T>), incomplete>
constexpr auto deduced_min() {
	return min_value<std::underlying_type_t<T>>;
}
template<typename T> requires std::is_same_v<decltype(max_value<T>), incomplete>
constexpr auto deduced_max() {
	return max_value<std::underlying_type_t<T>>;
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
