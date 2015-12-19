// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/common_type.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/arithmetic/minus.hpp>
#include <bounded/detail/arithmetic/modulus.hpp>
#include <bounded/detail/arithmetic/plus.hpp>

#include <type_traits>

namespace bounded {

struct modulo_policy {
	constexpr modulo_policy() noexcept {}

private:
	template<typename T, typename Size>
	static constexpr auto positive_remainder(T && value, Size && size) noexcept {
		using result_type = std::common_type_t<std::decay_t<T>, std::decay_t<decltype(std::forward<T>(value) + std::forward<Size>(size))>>;
		return (value < constant<0>) ?
			result_type(std::forward<T>(value) + std::forward<Size>(size), non_check) :
			result_type(std::forward<T>(value), non_check)
		;
	}

public:
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept {
		static_assert(is_bounded_integer<Minimum>, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<Maximum>, "Only bounded::integer types are supported.");
		return positive_remainder(
			(value - minimum) % (maximum - minimum + constant<1>),
			maximum - minimum + constant<1>
		) + minimum;
	}

	using overflow_policy_tag = void;
	static constexpr bool is_modulo = true;
	static constexpr bool overflow_is_error = false;
};

}	// namespace bounded
