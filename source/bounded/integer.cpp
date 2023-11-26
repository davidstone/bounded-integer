// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <bounded/conditional.hpp>

export module bounded.integer;

import bounded.bounded_integer;
import bounded.builtin_min_max_value;
import bounded.comparison;
import bounded.concepts;
import bounded.homogeneous_equals;
import bounded.is_bounded_integer;
import bounded.normalize;
import bounded.overlapping_range;
import bounded.safe_extreme;
import bounded.unchecked;
import bounded.underlying_type;

import numeric_traits;
import std_module;

namespace bounded {

export template<auto minimum, auto maximum>
struct integer;

} // namespace bounded

template<auto minimum, auto maximum>
constexpr auto numeric_traits::max_value<bounded::integer<minimum, maximum>> = bounded::integer<maximum, maximum>();

template<auto minimum, auto maximum>
constexpr auto numeric_traits::min_value<bounded::integer<minimum, maximum>> = bounded::integer<minimum, minimum>();

namespace bounded {

template<auto minimum, auto maximum>
constexpr auto is_bounded_integer<integer<minimum, maximum>> = true;

template<auto minimum, auto maximum>
constexpr auto builtin_max_value<integer<minimum, maximum>> = maximum;

template<auto minimum, auto maximum>
constexpr auto builtin_min_value<integer<minimum, maximum>> = minimum;

} // namespace bounded

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<bounded::bounded_integer LHS, bounded::bounded_integer RHS> requires std::same_as<LHS, std::decay_t<LHS>> and std::same_as<RHS, std::decay_t<RHS>>
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

template<typename T, auto minimum, auto maximum>
concept overlapping_integer =
	isomorphic_to_integral<T> and
	detail::safe_compare(builtin_min_value<T>, maximum) <= 0 and detail::safe_compare(minimum, builtin_max_value<T>) <= 0;

template<typename T, auto minimum, auto maximum>
concept bounded_by_range = overlapping_integer<T, minimum, maximum> and detail::safe_compare(minimum, builtin_min_value<T>) <= 0 and detail::safe_compare(builtin_max_value<T>, maximum) <= 0;

namespace detail {

export template<integral T, integral Minimum, integral Maximum>
constexpr auto assume_in_range_impl(T const value, Minimum, Maximum) {
	return integer<
		normalize<detail::safe_max(builtin_min_value<Minimum>, builtin_min_value<T>)>,
		normalize<detail::safe_min(builtin_max_value<Maximum>, builtin_max_value<T>)>
	>(value, unchecked);
}

} // namespace detail

export constexpr auto assume_in_range(integral auto const value, integral auto const minimum, integral auto const maximum) {
	if (value < minimum or maximum < value) {
		std::unreachable();
	}
	return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
}

export template<integral Target>
constexpr auto assume_in_range(integral auto const value) {
	constexpr auto minimum = numeric_traits::min_value<Target>;
	constexpr auto maximum = numeric_traits::max_value<Target>;
	if (integer(value) < integer(minimum) or integer(maximum) < integer(value)) {
		std::unreachable();
	}
	if constexpr (bounded_integer<Target>) {
		return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
	} else {
		return static_cast<Target>(value);
	}
}


export template<auto value>
using constant_t = integer<value, value>;

export template<auto value>
constexpr auto constant = constant_t<normalize<value>>();


// TODO: Report clang bug requiring this specialization
template<auto minimum>
struct integer<minimum, minimum> {
	static_assert(std::same_as<decltype(minimum), std::remove_const_t<decltype(normalize<minimum>)>>);

	using underlying_type = detail::underlying_type_t<minimum, minimum>;
	
	static constexpr auto value() -> underlying_type {
		return static_cast<underlying_type>(minimum);
	}
	
	integer() = default;
	constexpr integer(integer const &) = default;
	constexpr integer(integer &&) = default;

	constexpr integer(overlapping_integer<minimum, minimum> auto, unchecked_t) {
	}

	template<bounded_by_range<minimum, minimum> T>
	constexpr explicit(std::is_enum_v<T>) integer(T) {
	}

	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	constexpr auto operator=(overlapping_integer<minimum, minimum> auto const other) & -> integer & {
		if (other != constant<minimum>) {
			std::unreachable();
		}
		return *this;
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (builtin_integer<T> or std::is_enum_v<T> or std::floating_point<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}

	friend constexpr auto operator<=>(integer, integer) = default;
};

template<auto minimum, auto maximum>
struct integer {
	static_assert(std::same_as<decltype(minimum), std::remove_const_t<decltype(normalize<minimum>)>>);
	static_assert(std::same_as<decltype(maximum), std::remove_const_t<decltype(normalize<maximum>)>>);
	static_assert(detail::safe_compare(minimum, maximum) <= 0, "Maximum cannot be less than minimum");

	using underlying_type = detail::underlying_type_t<minimum, maximum>;
	
	constexpr auto value() const -> underlying_type {
		BOUNDED_ASSUME(minimum <= m_value and m_value <= maximum);
		return m_value;
	}
	
	integer() = default;
	constexpr integer(integer const &) = default;
	constexpr integer(integer &&) = default;

	constexpr integer(overlapping_integer<minimum, maximum> auto const other, unchecked_t):
		m_value(underlying_type(other))
	{
	}

	template<bounded_by_range<minimum, maximum> T>
	constexpr explicit(std::same_as<T, bool> or std::is_enum_v<T>) integer(T const other):
		m_value(underlying_type(other))
	{
	}

	constexpr auto operator=(integer const & other) & -> integer & = default;
	constexpr auto operator=(integer && other) & -> integer & = default;

	constexpr auto operator=(overlapping_integer<minimum, maximum> auto const other) & -> integer & {
		return *this = integer(::bounded::assume_in_range(other, constant<minimum>, constant<maximum>));
	}
	
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded::integer. It is subject to all
	// the standard rules of conversion from one integer type to another.
	template<typename T> requires (builtin_integer<T> or std::is_enum_v<T> or std::floating_point<T>)
	constexpr explicit operator T() const {
		return static_cast<T>(value());
	}

	friend constexpr auto operator<=>(integer, integer) = default;

public:
	// Do not access this directly. This must be public to be usable as a
	// non-type template parameter in C++20.
	[[no_unique_address]] underlying_type m_value;
};

template<typename T>
constexpr auto deduced_min = builtin_min_value<T>;

template<typename T>
constexpr auto deduced_max = builtin_max_value<T>;

template<typename T> requires(!numeric_traits::has_min_value<T>)
constexpr auto deduced_min<T> = builtin_min_value<std::underlying_type_t<T>>;

template<typename T> requires(!numeric_traits::has_max_value<T>)
constexpr auto deduced_max<T> = builtin_max_value<std::underlying_type_t<T>>;

template<typename T>
integer(T value) -> integer<deduced_min<T>, deduced_max<T>>;

} // namespace bounded

static_assert(bounded::bounded_integer<bounded::integer<0, 0>>);
static_assert(bounded::bounded_integer<bounded::integer<0, 1>>);

static_assert(bounded::integral<bounded::integer<0, 0>>);
static_assert(bounded::integral<bounded::integer<0, 1>>);

static_assert(bounded::isomorphic_to_integral<bounded::integer<0, 0>>);
static_assert(bounded::isomorphic_to_integral<bounded::integer<0, 1>>);

static_assert(bounded::assume_in_range(bounded::constant<5>, bounded::constant<0>, bounded::constant<10>) == bounded::constant<5>);
// This should not compile
// constexpr auto value2 = bounded::assume_in_range(15, bounded::constant<0>, bounded::constant<10>);
static_assert(bounded::assume_in_range(2, 1, 3) == bounded::integer(2));

static_assert(homogeneous_equals(bounded::normalize<bounded::constant<0>>, 0));

using type1 = bounded::integer<1, 5>;
using type2 = bounded::integer<3, 10>;
static_assert(std::same_as<std::common_type_t<type1, type2>, bounded::integer<1, 10>>);
using type3 = bounded::integer<-5, -5>;
static_assert(std::same_as<std::common_type_t<type1, type2, type3>, bounded::integer<-5, 10>>);
using type4 = bounded::integer<0, 0>;
static_assert(std::same_as<std::common_type_t<type1, type2, type3, type4>, bounded::integer<-5, 10>>);

static_assert(bounded::overlapping_integer<bounded::integer<0, 0>, 0, 0>, "Type should overlap its own range.");
static_assert(!bounded::overlapping_integer<bounded::integer<0, 0>, 1, 1>, "Type should not overlap a disjoint range.");

static_assert(bounded::bounded_by_range<bounded::integer<0, 0>, -1, 1>);
static_assert(!bounded::bounded_by_range<bounded::integer<0, 1>, 0, 0>);

static_assert(std::is_empty_v<bounded::constant_t<0>>);
static_assert(std::is_empty_v<bounded::constant_t<1>>);
static_assert(std::is_empty_v<bounded::constant_t<-1>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_min_value<numeric_traits::max_signed_t>>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_max_value<numeric_traits::max_signed_t>>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_max_value<numeric_traits::max_unsigned_t>>>);

static_assert(!bounded::convertible_to<bool, bounded::integer<0, 1>>);
static_assert(bounded::constructible_from<bounded::integer<0, 1>, bool>);
static_assert(!bounded::constructible_from<bounded::integer<0, 0>, bool>);

namespace check_constructibility {
	constexpr auto min = numeric_traits::min_value<int>;
	constexpr auto max = numeric_traits::max_value<int>;
	using type = bounded::integer<min, max>;
	static_assert(
		bounded::overlapping_integer<type, min, max>,
		"Bounds of type do not overlap its own range."
	);

	static_assert(
		bounded::convertible_to<int, type>,
		"Cannot convert integer type to bounded::integer with same range."
	);
	static_assert(
		bounded::constructible_from<type, type, bounded::unchecked_t>,
		"Cannot construct a type from itself with unchecked_t."
	);

	static_assert(
		!bounded::convertible_to<bool, type>,
		"Should not be able to convert a bool to a bounded::integer."
	);
	static_assert(
		bounded::constructible_from<type, bool>,
		"Should be able to construct a bounded::integer from a bool."
	);
}

static_assert(homogeneous_equals(
	BOUNDED_CONDITIONAL(true, bounded::constant<7>, bounded::constant<9>),
	bounded::integer<7, 9>(bounded::constant<7>)
));

constexpr auto check_assignment() {
	bounded::integer<0, 10> x(bounded::constant<5>);
	static_assert(!std::is_assignable_v<decltype((x)), bounded::constant_t<11>>);
	x = bounded::integer<10, 11>(bounded::constant<10>);
	BOUNDED_ASSERT(x == bounded::constant<10>);
	return true;
}

static_assert(check_assignment());


enum class bounded_enum{};

template<std::same_as<bounded_enum> T>
constexpr auto numeric_traits::min_value<T> = 0;

template<std::same_as<bounded_enum> T>
constexpr auto numeric_traits::max_value<T> = 0;

enum unscoped_enum : int {};
static_assert(!bounded::constructible_from<bounded::integer<0, 10>, unscoped_enum>);
static_assert(!bounded::convertible_to<unscoped_enum, bounded::integer<0, 10>>);

enum class scoped_enum {};
static_assert(!bounded::constructible_from<bounded::integer<0, 10>, scoped_enum>);
static_assert(!bounded::convertible_to<scoped_enum, bounded::integer<0, 10>>);

static_assert(bounded::constructible_from<bounded::integer<0, 10>, bounded_enum>);
static_assert(!bounded::convertible_to<bounded_enum, bounded::integer<0, 10>>);
static_assert(bounded::integer(bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_enum{}> == bounded::constant<0>);

enum class bounded_integer_enum{};

template<std::same_as<bounded_integer_enum> T>
constexpr auto numeric_traits::min_value<T> = bounded::constant<0>;

template<std::same_as<bounded_integer_enum> T>
constexpr auto numeric_traits::max_value<T> = bounded::constant<0>;

static_assert(!bounded::convertible_to<bounded_integer_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(bounded_integer_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_integer_enum{}> == bounded::constant<0>);

enum class enum_bounded_enum{};

template<std::same_as<enum_bounded_enum> T>
constexpr auto numeric_traits::min_value<T> = enum_bounded_enum();

template<std::same_as<enum_bounded_enum> T>
constexpr auto numeric_traits::max_value<T> = enum_bounded_enum();

static_assert(!bounded::convertible_to<enum_bounded_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(enum_bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<enum_bounded_enum{}> == bounded::constant<0>);

auto check_compile_time_comparison_of_runtime_constant() -> void {
	auto one = bounded::constant<1>;
	auto two = bounded::constant<2>;
	static_assert(one == one);
	static_assert(one <=> one == 0);
	static_assert(one != two);
	static_assert(one <=> two < 0);
}