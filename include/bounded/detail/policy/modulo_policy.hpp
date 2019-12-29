// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>

#include <type_traits>

namespace bounded {

struct modulo_policy {
private:
	static constexpr auto positive_remainder(auto const & value, auto const & size) {
		using result_type = std::common_type_t<decltype(value), decltype(value + size)>;
		return (value < constant<0>) ?
			result_type(value + size, non_check) :
			result_type(value, non_check)
		;
	}

public:
	static constexpr auto assignment(bounded_integer auto const & value, bounded_integer auto const & minimum, bounded_integer auto const & maximum) {
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
