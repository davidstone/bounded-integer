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
	static constexpr auto positive_remainder(T const & value, Size const & size) noexcept {
		using result_type = std::common_type_t<T, decltype(value + size)>;
		return (value < constant<0>) ?
			result_type(value + size, non_check) :
			result_type(value, non_check)
		;
	}

public:
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T const & value, Minimum const & minimum, Maximum const & maximum) noexcept {
		static_assert(bounded_integer<Minimum>, "Only bounded::integer types are supported.");
		static_assert(bounded_integer<Maximum>, "Only bounded::integer types are supported.");
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
