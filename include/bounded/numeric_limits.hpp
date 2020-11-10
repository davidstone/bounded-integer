// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/log.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/minmax.hpp>

#include <limits>

namespace bounded {
namespace detail {

// https://stackoverflow.com/questions/19609186/what-is-stdnumeric-limitstdigits-supposed-to-represent
// digits and digits10 represent the maximum number of bits / digits in a
// lossless string -> integer -> string conversion sequence

constexpr auto digits(auto const minimum, auto const maximum, auto const base) noexcept {
	static_assert(base > constant<1>, "Base must be greater than 1.");
	if constexpr (minimum > constant<0> or maximum <= constant<0>) {
		return constant<0>;
	} else if constexpr (maximum == constant<max_value<max_unsigned_t>>) {
		if constexpr (base == constant<2>) {
			return constant<sizeof(max_unsigned_t) * CHAR_BIT>;
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

}	// namespace detail
}	// namespace bounded
namespace std {

template<auto minimum, auto maximum>
struct numeric_limits<bounded::integer<minimum, maximum>> {
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
	static constexpr auto digits = bounded::detail::digits(bounded::constant<minimum>, bounded::constant<maximum>, bounded::constant<radix>);
	static constexpr auto digits10 = bounded::detail::digits(bounded::constant<minimum>, bounded::constant<maximum>, bounded::constant<10>);
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

}	// namespace std
