// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <numeric_traits/has_int128.hpp>

export module bounded.arithmetic.unary_minus;

import bounded.homogeneous_equals;
import bounded.integer;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

template<auto value>
constexpr auto safer_negation() {
	constexpr auto negated = -static_cast<numeric_traits::max_unsigned_t>(value);
	if constexpr (value <= 0) {
		return negated;
	} else {
		constexpr auto max_positive_input = -static_cast<numeric_traits::max_unsigned_t>(numeric_traits::min_value<numeric_traits::max_signed_t>);
		static_assert(value <= max_positive_input, "Negation would overflow.");
		return static_cast<numeric_traits::max_signed_t>(negated);
	}
}

export template<auto minimum, auto maximum>
constexpr auto operator-(integer<minimum, maximum> const value) {
	using result_type = integer<
		normalize<safer_negation<maximum>()>,
		normalize<safer_negation<minimum>()>
	>;
	using underlying_type = typename result_type::underlying_type;
	using promoted_unsigned_type = numeric_traits::promoted_unsigned<underlying_type>;
	return result_type(
		static_cast<underlying_type>(-static_cast<promoted_unsigned_type>(value)),
		unchecked
	);
}

} // namespace bounded

namespace {

static_assert(homogeneous_equals(
	-bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	-bounded::constant<1>,
	bounded::constant<-1>
));

template<typename Signed, typename Unsigned>
constexpr auto check_signed_limits() {
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::max_value<Signed>>,
		bounded::constant<-numeric_traits::max_value<Signed>>
	));
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::min_value<Signed>>,
		// The outer cast to unsigned is to counteract integer promotion
		bounded::constant<static_cast<Unsigned>(-static_cast<Unsigned>(numeric_traits::min_value<Signed>))>
	));
	static_assert(homogeneous_equals(
		-(-bounded::constant<numeric_traits::min_value<Signed>>),
		bounded::constant<numeric_traits::min_value<Signed>>
	));
}
template<typename Signed, typename Unsigned, typename LargerSigned>
constexpr auto check_limits() {
	check_signed_limits<Signed, Unsigned>();
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::max_value<Unsigned>>,
		bounded::constant<-static_cast<LargerSigned>(numeric_traits::max_value<Unsigned>)>
	));
}

constexpr auto check_all_limits() {
	check_limits<std::int8_t, std::uint8_t, std::int16_t>();
	check_limits<std::int16_t, std::uint16_t, std::int32_t>();
	check_limits<std::int32_t, std::uint32_t, std::int64_t>();
	#if defined NUMERIC_TRAITS_HAS_INT128
	check_limits<std::int64_t, std::uint64_t, numeric_traits::int128_t>();
	#else
	check_signed_limits<std::int64_t, std::uint64_t>();
	#endif
	return true;
}
static_assert(check_all_limits());

static_assert(homogeneous_equals(
	-bounded::integer<1, 10>(bounded::constant<9>),
	bounded::integer<-10, -1>(bounded::constant<-9>)
));

static_assert(homogeneous_equals(
	-bounded::integer<-3, 6>(bounded::constant<4>),
	bounded::integer<-6, 3>(bounded::constant<-4>)
));

} // namespace