// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/min_max_value.hpp>

#include <limits>
#include <type_traits>

namespace bounded {
namespace detail {

// https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior/57947296#57947296
// has the full explanation for this code
template<detail_unsigned_builtin T>
constexpr auto cast_to_signed_integer(T const value) {
	using unsigned_t = std::conditional_t<sizeof(T) <= sizeof(unsigned), unsigned, T>;
	using signed_t = std::make_signed_t<unsigned_t>;
	using result_t = std::make_signed_t<T>;
	if (value <= max_value<result_t>) {
		return static_cast<result_t>(value);
	} else {
		constexpr auto window = static_cast<signed_t>(min_value<result_t>);
		return static_cast<result_t>( // cast to the type we want the result in
			static_cast<signed_t>( // cast back to signed or we end up with our original value
				static_cast<T>( // cast to unsigned to force modular reduction
					static_cast<unsigned_t>(value) + // cast to avoid promotion to int
					static_cast<unsigned_t>(window) // shift values to overlapping range, cast to silence warning
				)
			) + window // shift values to negative range
		);
	}
}

constexpr auto cast_to_signed_integer(detail_signed_builtin auto const value) {
	return value;
}

// Returns the equivalent value under modular arithmetic
template<typename Result, typename Source>
constexpr auto modulo_cast(Source const source) {
	if constexpr (std::is_signed_v<Result>) {
		return ::bounded::detail::cast_to_signed_integer(static_cast<std::make_unsigned_t<Result>>(source));
	} else {
		return static_cast<Result>(source);
	}
}

} // namespace detail
} // namespace bounded
