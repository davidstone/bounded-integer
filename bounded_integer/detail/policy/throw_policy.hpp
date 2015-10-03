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

#include "../comparison.hpp"
#include "../is_bounded_integer.hpp"
#include "../string.hpp"
#include <stdexcept>

namespace bounded {
namespace policy_detail {

using default_exception = std::range_error;

}	// namespace policy_detail

template<typename Exception = policy_detail::default_exception>
struct throw_policy {
	constexpr throw_policy() noexcept {}

	// The optimizer should be able to simplify this to remove dead checks.
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) -> T && {
		static_assert(is_bounded_integer<std::decay_t<Minimum>>::value, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<std::decay_t<Maximum>>::value, "Only bounded::integer types are supported.");
		return (minimum <= value and value <= maximum) ?
			std::forward<T>(value) :
			(throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]"), std::forward<T>(value));
	}
	
	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
};

}	// namespace bounded
