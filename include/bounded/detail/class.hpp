// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/assume.hpp>
#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/overlapping_range.hpp>
#include <bounded/detail/safe_extreme.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <bounded/assume_in_range.hpp>
#include <bounded/concepts.hpp>
#include <bounded/normalize.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <type_traits>
#include <utility>

namespace bounded {

template<auto minimum, auto maximum>
struct integer;

} // namespace bounded

template<auto minimum, auto maximum>
inline constexpr auto numeric_traits::max_value<bounded::integer<minimum, maximum>> = bounded::integer<maximum, maximum>();

template<auto minimum, auto maximum>
inline constexpr auto numeric_traits::min_value<bounded::integer<minimum, maximum>> = bounded::integer<minimum, minimum>();

namespace bounded {

template<auto minimum, auto maximum>
inline constexpr auto builtin_max_value<integer<minimum, maximum>> = maximum;

template<auto minimum, auto maximum>
inline constexpr auto builtin_min_value<integer<minimum, maximum>> = minimum;

} // namespace bounded

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<bounded::bounded_integer LHS, bounded::bounded_integer RHS> requires(std::is_same_v<LHS, std::decay_t<LHS>> and std::is_same_v<RHS, std::decay_t<RHS>>)
struct std::common_type<LHS, RHS> {
private:
	static inline constexpr auto minimum = bounded::normalize<bounded::detail::safe_min(
		bounded::builtin_min_value<LHS>,
		bounded::builtin_min_value<RHS>
	)>;
	static inline constexpr auto maximum = bounded::normalize<bounded::detail::safe_max(
		bounded::builtin_max_value<LHS>,
		bounded::builtin_max_value<RHS>
	)>;
public:
	using type = bounded::integer<minimum, maximum>;
};

namespace bounded {


// Use non_check_t constructors if you know by means that cannot be determined
// by the type system that the value fits in the range.
inline constexpr struct non_check_t{} non_check;

namespace detail {

template<typename T, auto minimum, auto maximum>
concept overlapping_integer =
	(integral<T> or std::is_enum_v<T> or std::is_same_v<T, bool>) and
	safe_compare(builtin_min_value<T>, maximum) <= 0 and safe_compare(minimum, builtin_max_value<T>) <= 0;

template<typename T, auto minimum, auto maximum>
concept bounded_by_range = !std::is_same_v<T, bool> and !std::is_enum_v<T> and overlapping_integer<T, minimum, maximum> and safe_compare(minimum, builtin_min_value<T>) <= 0 and safe_compare(builtin_max_value<T>, maximum) <= 0;


// Cannot use CTAD in the constructor
template<typename T>
constexpr auto as_integer(T const value) {
	using result_type = integer<
		builtin_min_value<T>,
		builtin_max_value<T>
	>;
	return result_type(value, non_check);
}


struct empty {
	constexpr empty() = default;
	constexpr explicit empty(auto) {
	}
	friend constexpr auto operator<=>(empty, empty) = default;
};

}	// namespace detail


template<auto value>
using constant_t = integer<value, value>;

template<auto value>
inline constexpr auto constant = constant_t<normalize<value>>{};


template<auto minimum, auto maximum>
struct integer {
	static_assert(std::is_same_v<decltype(minimum), std::remove_const_t<decltype(normalize<minimum>)>>);
	static_assert(std::is_same_v<decltype(maximum), std::remove_const_t<decltype(normalize<maximum>)>>);
	static_assert(detail::safe_compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = detail::underlying_type_t<minimum, maximum>;
	
	constexpr auto value() const {
		BOUNDED_ASSUME(minimum <= m_value and m_value <= maximum);
		return m_value;
	}
	static constexpr auto value() requires(minimum == maximum) {
		return static_cast<underlying_type>(minimum);
	}
	
	integer() = default;
	constexpr integer(integer const &) = default;
	constexpr integer(integer &&) = default;

	constexpr integer(detail::overlapping_integer<minimum, maximum> auto const other, non_check_t):
		m_value(static_cast<underlying_type>(other))
	{
	}

	constexpr integer(detail::bounded_by_range<minimum, maximum> auto const other):
		m_value(static_cast<underlying_type>(other))
	{
	}

	template<detail::overlapping_integer<minimum, maximum> T>
	constexpr explicit integer(T const other):
		m_value(
			::bounded::assume_in_range(
				detail::as_integer(other),
				constant<minimum>,
				constant<maximum>
			)
		)
	{
	}

	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::overlapping_integer<Enum, minimum, maximum>
	)
	constexpr integer(Enum other, non_check_t):
		m_value(static_cast<underlying_type>(other))
	{
	}
	template<typename Enum> requires(
		std::is_enum_v<Enum> and !detail::overlapping_integer<Enum, minimum, maximum>
	)
	constexpr explicit integer(Enum other):
		integer(std::to_underlying(other))
	{
	}


	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	constexpr auto && operator=(detail::overlapping_integer<minimum, maximum> auto const other) & {
		return *this = integer(other);
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (detail::builtin_arithmetic<T> or std::is_enum_v<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}

	friend constexpr auto operator<=>(integer, integer) = default;
	
private:
	using storage_type = std::conditional_t<minimum == maximum, detail::empty, underlying_type>;

public:
	// Do not access this directly. This must be public to be usable as a
	// non-type template parameter in C++20.
	[[no_unique_address]] storage_type m_value;
};


namespace detail {

template<typename T>
inline constexpr auto deduced_min = builtin_min_value<T>;

template<typename T>
inline constexpr auto deduced_max = builtin_max_value<T>;

template<typename T> requires(!numeric_traits::has_min_value<T>)
inline constexpr auto deduced_min<T> = builtin_min_value<std::underlying_type_t<T>>;

template<typename T> requires(!numeric_traits::has_max_value<T>)
inline constexpr auto deduced_max<T> = builtin_max_value<std::underlying_type_t<T>>;

}	// namespace detail

template<typename T>
integer(T value) -> integer<
	detail::deduced_min<T>,
	detail::deduced_max<T>
>;

}	// namespace bounded
