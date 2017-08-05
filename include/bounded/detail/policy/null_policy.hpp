// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <utility>

namespace bounded {

// Does nothing. Overflow is undefined behavior.
struct null_policy {
	constexpr null_policy() noexcept {}
	
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// https://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept -> T && {
		static_assert(is_bounded_integer<Minimum>, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<Maximum>, "Only bounded::integer types are supported.");
		return (minimum <= value and value <= maximum) ?
			std::forward<T>(value) :
			error_out_of_range(std::forward<T>(value));
	}

	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
private:
	template<typename T>
	static auto error_out_of_range(T && value) noexcept -> T && {
		return std::forward<T>(value);
	}
};


}	// namespace bounded
