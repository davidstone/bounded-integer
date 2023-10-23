// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.base;

import bounded.integer;
import bounded.is_bounded_integer;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {
namespace detail {

export template<typename Min, typename Max>
struct min_max {
	Min min;
	Max max;
};
template<typename Min, typename Max>
min_max(Min min, Max max) -> min_max<Min, Max>;

template<typename T>
constexpr auto min_max_range = min_max{numeric_traits::min_value<T>, numeric_traits::max_value<T>};

export template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator_overload(LHS const lhs, RHS const rhs, auto const arithmetic_function, auto const operator_range) {
	constexpr auto range = operator_range(min_max_range<LHS>, min_max_range<RHS>);
	using result_t = integer<normalize<range.min>, normalize<range.max>>;
	using common_t = typename std::common_type_t<result_t, LHS, RHS>::underlying_type;
	// It is safe to use the unchecked constructor because we already know that
	// the result will fit in result_t. We have to cast to the intermediate
	// common_t in case result_t is narrower than one of the arguments.
	return result_t(
		arithmetic_function(static_cast<common_t>(lhs), static_cast<common_t>(rhs)),
		unchecked
	);
}

export template<bounded_integer LHS, bounded_integer RHS>
constexpr auto modulo_equivalent_operator_overload(LHS const lhs, RHS const rhs, auto const arithmetic_function, auto const operator_range) {
	constexpr auto range = operator_range(min_max_range<LHS>, min_max_range<RHS>);
	using result_t = integer<normalize<range.min>, normalize<range.max>>;
	using intermediate_t = numeric_traits::make_unsigned<std::common_type_t<
		typename result_t::underlying_type,
		typename LHS::underlying_type,
		typename RHS::underlying_type
	>>;
	auto const intermediate = arithmetic_function(
		static_cast<intermediate_t>(lhs),
		static_cast<intermediate_t>(rhs)
	);
	return result_t(
		static_cast<typename result_t::underlying_type>(intermediate),
		unchecked
	);
}

} // namespace detail
} // namespace bounded
