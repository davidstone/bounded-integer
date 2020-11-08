// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/assume.hpp>
#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/min_max_value.hpp>
#include <bounded/detail/normalize.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <bounded/detail/policy/null_policy.hpp>

#include <bounded/concepts.hpp>

#include <limits>
#include <type_traits>
#include <utility>

namespace bounded {

template<auto minimum, auto maximum, typename overflow_policy>
struct integer;

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto max_value<integer<minimum, maximum, overflow_policy>> = integer<maximum, maximum, null_policy>();

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto min_value<integer<minimum, maximum, overflow_policy>> = integer<minimum, minimum, null_policy>();

namespace detail {

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto is_bounded_integer<integer<minimum, maximum, overflow_policy>> = true;

template<auto minimum, auto maximum, typename policy>
inline constexpr auto builtin_max_value<integer<minimum, maximum, policy>> = maximum;

template<auto minimum, auto maximum, typename policy>
inline constexpr auto builtin_min_value<integer<minimum, maximum, policy>> = minimum;

} // namespace detail
} // namespace bounded
namespace std {

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<bounded::bounded_integer LHS, bounded::bounded_integer RHS> requires(std::is_same_v<LHS, std::decay_t<LHS>> and std::is_same_v<RHS, std::decay_t<RHS>>)
struct common_type<LHS, RHS> {
private:
	static inline constexpr auto minimum = bounded::detail::normalize<bounded::detail::safe_min(
		bounded::detail::builtin_min_value<LHS>,
		bounded::detail::builtin_min_value<RHS>
	)>;
	static inline constexpr auto maximum = bounded::detail::normalize<bounded::detail::safe_max(
		bounded::detail::builtin_max_value<LHS>,
		bounded::detail::builtin_max_value<RHS>
	)>;
public:
	using type = bounded::integer<minimum, maximum, bounded::null_policy>;
};

} // namespace std
namespace bounded {

template<typename T>
inline constexpr auto is_integer = detail::builtin_integer<T> or bounded_integer<T>;

template<typename T, T value>
inline constexpr auto is_integer<std::integral_constant<T, value>> = is_integer<T>;

template<typename T>
concept integral = is_integer<T>;


// Use non_check_t constructors if you know by means that cannot be determined
// by the type system that the value fits in the range.
inline constexpr struct non_check_t{} non_check;

namespace detail {

template<typename T, auto minimum, auto maximum, typename policy>
concept overlapping_integer =
	(is_integer<T> or std::is_enum_v<T> or std::is_same_v<T, bool>) and
	(!policy::overflow_is_error or (safe_compare(builtin_min_value<T>, maximum) <= 0 and safe_compare(minimum, builtin_max_value<T>) <= 0));

template<typename T, auto minimum, auto maximum, typename policy>
concept bounded_by_range = !std::is_same_v<T, bool> and !std::is_enum_v<T> and overlapping_integer<T, minimum, maximum, policy> and safe_compare(minimum, builtin_min_value<T>) <= 0 and safe_compare(builtin_max_value<T>, maximum) <= 0;


// The user can specialize min_value and max_value for their enum type to
// provide tighter bounds than the underlying_type might suggest. This forwards
// along non-enum types without doing anything, but constructs a
// bounded::integer with the tighter bounds from an enumeration.
template<typename T>
constexpr decltype(auto) as_integer(T const & value) {
	if constexpr (std::is_enum_v<T>) {
		using result_type = integer<
			builtin_min_value<T>,
			builtin_max_value<T>,
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


template<auto minimum, auto maximum, typename overflow_policy = null_policy>
struct integer {
	static_assert(std::is_same_v<decltype(minimum), std::remove_const_t<decltype(detail::normalize<minimum>)>>);
	static_assert(std::is_same_v<decltype(maximum), std::remove_const_t<decltype(detail::normalize<maximum>)>>);
	static_assert(detail::safe_compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = detail::underlying_type_t<minimum, maximum>;
	
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

	constexpr integer(detail::overlapping_integer<minimum, maximum, overflow_policy> auto const & other, non_check_t):
		m_value(static_cast<underlying_type>(other)) {
	}

	constexpr integer(detail::bounded_by_range<minimum, maximum, overflow_policy> auto const other, overflow_policy = overflow_policy{}):
		integer(other, non_check)
	{
	}

	template<detail::overlapping_integer<minimum, maximum, overflow_policy> T> requires(!detail::bounded_by_range<T, minimum, maximum, overflow_policy>)
	constexpr explicit integer(T const & other, overflow_policy = overflow_policy{}):
		integer(
			overflow_policy().assignment(
				integer<detail::builtin_min_value<T>, detail::builtin_max_value<T>>(detail::as_integer(other), non_check),
				constant<minimum>,
				constant<maximum>
			),
			non_check
		)
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


	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	constexpr auto && operator=(detail::overlapping_integer<minimum, maximum, overflow_policy> auto const & other) & {
		return *this = integer(other);
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (detail::builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}
	
private:
	static constexpr auto uninitialized_value() {
		return minimum > min_value<underlying_type> ?
			static_cast<underlying_type>(minimum - 1) : static_cast<underlying_type>(maximum + 1);
	}

	using storage_type = std::conditional_t<minimum == maximum, detail::empty, underlying_type>;

public:
	// Do not access this directly. This must be public to be usable as a
	// non-type template parameter in C++20.
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
