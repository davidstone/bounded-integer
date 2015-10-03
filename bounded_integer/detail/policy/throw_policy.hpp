// Throws an exception on overflow
// Copyright (C) 2015 David Stone
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

#pragma once

#include "null_policy.hpp"
#include "../comparison.hpp"
#include "../minmax.hpp"
#include "../noexcept.hpp"
#include "../numeric_limits.hpp"
#include "../is_bounded_integer.hpp"
#include "../string.hpp"
#include <stdexcept>

namespace bounded {
namespace policy_detail {

using default_exception = std::range_error;

template<intmax_t minimum, intmax_t maximum, typename T>
constexpr auto reduce_range(T && value) BOUNDED_NOEXCEPT(
	integer<
		max(minimum, basic_numeric_limits<T>::min()),
		min(maximum, basic_numeric_limits<T>::max()),
		null_policy,
		storage_type::fast
	>(std::forward<T>(value))
)

}	// namespace policy_detail

template<typename Exception = policy_detail::default_exception>
struct throw_policy {
	constexpr throw_policy() noexcept {}

	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) {
		static_assert(is_bounded_integer<std::decay_t<Minimum>>::value, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<std::decay_t<Maximum>>::value, "Only bounded::integer types are supported.");
		if (minimum <= value and value <= maximum) {
			return policy_detail::reduce_range<basic_numeric_limits<Minimum>::min(), basic_numeric_limits<Maximum>::max()>(std::forward<T>(value));
		}
		throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
	
	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded
