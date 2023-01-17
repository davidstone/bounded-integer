// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.numeric_limits;

import bounded.arithmetic.multiplies;
import bounded.arithmetic.plus;
import bounded.arithmetic.unary_minus;
import bounded.comparison;
import bounded.comparison_builtin;
import bounded.integer;
import bounded.log;
import bounded.minmax;
import bounded.normalize;
import bounded.size_of;

import numeric_traits;
import std_module;

namespace bounded {

// https://stackoverflow.com/questions/19609186/what-is-stdnumeric-limitstdigits-supposed-to-represent
// digits and digits10 represent the maximum number of bits / digits in a
// lossless string -> integer -> string conversion sequence

constexpr auto digits(auto const minimum, auto const maximum, auto const base) noexcept {
	static_assert(base > constant<1>, "Base must be greater than 1.");
	if constexpr (minimum > constant<0> or maximum <= constant<0>) {
		return constant<0>;
	} else if constexpr (maximum == constant<numeric_traits::max_value<numeric_traits::max_unsigned_t>>) {
		if constexpr (base == constant<2>) {
			return size_of_bits<numeric_traits::max_unsigned_t>;
		} else {
			// TODO: Not quite right, but avoids overflow
			return ::bounded::log(maximum, base);
		}
	} else if constexpr (minimum == constant<0> or maximum < -minimum) {
		return ::bounded::log(maximum + constant<1>, base);
	} else {
		return ::bounded::log(-minimum + constant<1>, base);
	}
}

} // namespace bounded

template<auto minimum, auto maximum>
struct std::numeric_limits<bounded::integer<minimum, maximum>> {
private:
	using type = bounded::integer<minimum, maximum>;
public:
	static constexpr auto is_specialized = true;
	static constexpr auto is_signed = minimum < 0;
	static constexpr auto is_integer = true;
	static constexpr auto is_exact = true;
	static constexpr auto has_infinity = false;
	static constexpr auto has_quiet_NaN = false;
	static constexpr auto has_signaling_NaN = false;
	static constexpr auto has_denorm = std::denorm_absent;
	static constexpr auto has_denorm_loss = false;
	static constexpr auto round_style = std::round_toward_zero;
	static constexpr auto is_iec559 = false;
	static constexpr auto is_bounded = true;
	// This is meaningless for all specializations
	static constexpr auto is_modulo = false;
	static constexpr auto radix = 2;
	static constexpr auto digits = bounded::digits(bounded::constant<minimum>, bounded::constant<maximum>, bounded::constant<radix>);
	static constexpr auto digits10 = bounded::digits(bounded::constant<minimum>, bounded::constant<maximum>, bounded::constant<10>);
	static constexpr auto max_digits10 = 0;
	static constexpr auto min_exponent = 0;
	static constexpr auto min_exponent10 = 0;
	static constexpr auto max_exponent = 0;
	static constexpr auto max_exponent10 = 0;

	// If 0 is not in range, there is no trap value for arithmetic
	static constexpr auto traps =
		minimum <= 0 and 0 <= maximum and
		numeric_limits<typename type::underlying_type>::traps;

	static constexpr auto tinyness_before = false;
	
	static constexpr auto min() noexcept {
		return bounded::constant<minimum>;
	}
	static constexpr auto lowest() noexcept {
		return bounded::constant<minimum>;
	}
	static constexpr auto max() noexcept {
		return bounded::constant<maximum>;
	}
	// 18.3.2.7.1 says:
	// Any value that is not "meaningful" shall be set to 0 or false.
	static constexpr auto epsilon() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto round_error() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto infinity() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto quiet_NaN() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto signaling_NaN() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto denorm_min() noexcept {
		return bounded::constant<0>;
	}
};

template<typename integer>
constexpr auto check_numeric_limits() {
	using int_limits = std::numeric_limits<integer>;
	using bounded_t = bounded::integer<
		bounded::normalize<int_limits::min()>,
		bounded::normalize<int_limits::max()>
	>;
	static_assert(std::same_as<typename bounded_t::underlying_type, integer>, "Incorrect underlying_type.");
	using bounded_limits = std::numeric_limits<bounded_t>;
	static_assert(sizeof(bounded_t) == sizeof(integer), "bounded::integer wrong size.");

	// I have to use the preprocessor here to create a string literal
	#define BOUNDED_INTEGER_CHECK_CONDITION(condition) \
		static_assert(int_limits::condition == bounded_limits::condition, #condition " is wrong.")
	BOUNDED_INTEGER_CHECK_CONDITION(is_specialized);
	BOUNDED_INTEGER_CHECK_CONDITION(is_integer);
	BOUNDED_INTEGER_CHECK_CONDITION(is_exact);
	BOUNDED_INTEGER_CHECK_CONDITION(has_infinity);
	BOUNDED_INTEGER_CHECK_CONDITION(has_quiet_NaN);
	BOUNDED_INTEGER_CHECK_CONDITION(has_signaling_NaN);
	BOUNDED_INTEGER_CHECK_CONDITION(has_denorm);
	BOUNDED_INTEGER_CHECK_CONDITION(has_denorm_loss);
	BOUNDED_INTEGER_CHECK_CONDITION(is_iec559);
	BOUNDED_INTEGER_CHECK_CONDITION(is_bounded);
	// is_modulo intentionally left out because it is meaningless
	BOUNDED_INTEGER_CHECK_CONDITION(radix);
	BOUNDED_INTEGER_CHECK_CONDITION(digits);
	BOUNDED_INTEGER_CHECK_CONDITION(digits10);
	BOUNDED_INTEGER_CHECK_CONDITION(max_digits10);
	BOUNDED_INTEGER_CHECK_CONDITION(min_exponent);
	BOUNDED_INTEGER_CHECK_CONDITION(min_exponent10);
	BOUNDED_INTEGER_CHECK_CONDITION(max_exponent);
	BOUNDED_INTEGER_CHECK_CONDITION(max_exponent10);
	BOUNDED_INTEGER_CHECK_CONDITION(traps);
	BOUNDED_INTEGER_CHECK_CONDITION(tinyness_before);
	#undef BOUNDED_INTEGER_CHECK_CONDITION

	#define BOUNDED_INTEGER_CHECK_FUNCTION(function) \
		static_assert(int_limits::function() == bounded_limits::function(), #function "() is wrong.")
	// Some of the functions are meaningless for integers, so I do not compare
	#define BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(function) \
		static_cast<void>(bounded_limits::function())
	BOUNDED_INTEGER_CHECK_FUNCTION(min);
	BOUNDED_INTEGER_CHECK_FUNCTION(lowest);
	BOUNDED_INTEGER_CHECK_FUNCTION(max);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(epsilon);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(round_error);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(infinity);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(quiet_NaN);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(signaling_NaN);
	BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION(denorm_min);
	#undef BOUNDED_INTEGER_CHECK_MEANINGLESS_FUNCTION
	#undef BOUNDED_INTEGER_CHECK_FUNCTION
}


constexpr auto check_numeric_limits_all() {
	static_assert(std::numeric_limits<bounded::integer<1, 1000>>::digits == bounded::constant<0>, "Meaningless digits not 0.");
	check_numeric_limits<std::int8_t>();
	check_numeric_limits<std::uint8_t>();
	check_numeric_limits<std::int16_t>();
	check_numeric_limits<std::uint16_t>();
	check_numeric_limits<std::int32_t>();
	check_numeric_limits<std::uint32_t>();
	check_numeric_limits<std::int64_t>();
	check_numeric_limits<std::uint64_t>();
	return true;
}

static_assert(check_numeric_limits_all());
