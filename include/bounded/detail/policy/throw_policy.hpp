// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/policy/null_policy.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_mixed.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/min_max_value.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/string.hpp>
#include <stdexcept>

namespace bounded {
namespace policy_detail {

using default_exception = std::range_error;

template<typename T, auto minimum, auto maximum>
constexpr auto reduce_range(T const & value, constant_t<minimum>, constant_t<maximum>) {
	return integer<
		detail::normalize<detail::safe_max(minimum, detail::builtin_min_value<T>)>,
		detail::normalize<detail::safe_min(maximum, detail::builtin_max_value<T>)>,
		null_policy
	>(value);
}

}	// namespace policy_detail

template<typename Exception = policy_detail::default_exception>
struct throw_policy {
	template<bounded_integer Minimum, bounded_integer Maximum>
	static constexpr auto assignment(auto const & value, Minimum const & minimum, Maximum const & maximum) {
		if (minimum <= value and value <= maximum) {
			return policy_detail::reduce_range(value, min_value<Minimum>, max_value<Maximum>);
		} else {
			throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
		}
	}
	
	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded
