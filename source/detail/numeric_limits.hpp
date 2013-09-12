// Specialization of numeric_limits
// Copyright (C) 2013 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef RANGED_INTEGER_NUMERIC_LIMITS_HPP_
#define RANGED_INTEGER_NUMERIC_LIMITS_HPP_

#include "forward_declaration.hpp"
#include <cmath>
#include <cstdint>
#include <limits>

namespace std {

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
class numeric_limits<ranged_integer<minimum, maximum, OverflowPolicy>> {
private:
	using type = ranged_integer<minimum, maximum, OverflowPolicy>;
	static constexpr int log2abs(intmax_t value, int sum = 0) {
		return (value == 0) ? sum : log2abs(value / 2, sum + 1);
	}
public:
	static constexpr bool is_specialized = true;
	static constexpr bool is_signed = minimum < 0;
	static constexpr bool is_integer = true;
	static constexpr bool is_exact = true;
	static constexpr bool has_infinity = false;
	static constexpr bool has_quiet_NaN = false;
	static constexpr bool has_signaling_NaN = false;
	static constexpr auto has_denorm = std::denorm_absent;
	static constexpr bool has_denorm_loss = false;
	static constexpr auto round_style = std::round_toward_zero;
	static constexpr bool is_iec559 = false;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = OverflowPolicy<maximum, minimum>::is_modulo;
	static constexpr int radix = 2;
	// I don't know if this is actually correct. I could theoretically use a
	// more compact representation of values that have a minimum that is far
	// from zero. For instance, it only takes a single bit to represent the
	// value in the range of 500 through 501, but my current implementation
	// would actually use 9 bits of space. I don't know what this value is
	// ultimately supposed to represent, and I may create a specialized version
	// to compact the representation.
	static constexpr int digits = ((log2abs(maximum) > log2abs(minimum)) ? log2abs(maximum) : log2abs(minimum)) - (is_signed ? 1 : 0);
	static constexpr int digits10 = digits * std::log10(radix);
	static constexpr int max_digits10 = 0;
	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;
	// If 0 is not in range, there is no trap value for arithmetic
	static constexpr bool traps = minimum <= 0 and 0 <= maximum;
	static constexpr bool tinyness_before = false;
	
	static constexpr type min() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type lowest() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type max() noexcept {
		return type(maximum, non_check);
	}
	// Some of these functions return 0 for built-in integer types, but 0 may
	// not be in the representable range. Fortunately, they are also defined as
	// only being meaningful for floating-point types, so we can just stick any
	// value in here.
	static constexpr type epsilon() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type round_error() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type infinity() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type quiet_NaN() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type signaling_NaN() noexcept {
		return type(minimum, non_check);
	}
	static constexpr type denorm_min() noexcept {
		return type(minimum, non_check);
	}
};

}	// namespace std

#endif	// RANGED_INTEGER_NUMERIC_LIMITS_HPP_
