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
#include <bounded/detail/minmax.hpp>

#include <cstdint>
#include <limits>

namespace bounded {
namespace detail {

// https://stackoverflow.com/questions/19609186/what-is-stdnumeric-limitstdigits-supposed-to-represent
// digits and digits10 represent the maximum number of bits / digits in a
// lossless string -> integer -> string conversion sequence

template<typename Minimum, typename Maximum, typename Base>
constexpr auto digits(Minimum const minimum, Maximum const maximum, Base const base) noexcept {
	static_assert(base > constant<1>, "Base must be greater than 1.");
	if constexpr (minimum > constant<0> or maximum <= constant<0>) {
		return constant<0>;
	} else if constexpr (maximum == constant<std::numeric_limits<std::uintmax_t>::max()>) {
		return constant<std::numeric_limits<std::uintmax_t>::digits>;
	} else if constexpr (minimum == constant<0> or maximum < -minimum) {
		return ::bounded::log(maximum + constant<1>, base);
	} else {
		return ::bounded::log(-minimum + constant<1>, base);
	}
}

}	// namespace detail
}	// namespace bounded
namespace std {

template<auto minimum, auto maximum, typename overflow_policy, bounded::storage_type storage, bool poisoned>
struct numeric_limits<bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>> {
private:
	using type = bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>;
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
	static constexpr auto is_modulo = overflow_policy::is_modulo;
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
		return bounded::constant<minimum, overflow_policy, storage>;
	}
	static constexpr auto lowest() noexcept {
		return bounded::constant<minimum, overflow_policy, storage>;
	}
	static constexpr auto max() noexcept {
		return bounded::constant<maximum, overflow_policy, storage>;
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
