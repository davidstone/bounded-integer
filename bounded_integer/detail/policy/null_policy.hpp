// Does nothing. Overflow is undefined behavior
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_

#include "basic_policy.hpp"
#include <cstdint>

namespace bounded_integer {
namespace policy_detail {

class null_policy {
public:
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// http://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<typename T, typename Minimum, typename Maximum>
	static constexpr intmax_t assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept {
		return (minimum <= value and value <= maximum) ?
			static_cast<intmax_t>(value) :
			static_cast<intmax_t>(error_out_of_range(value));
	}

	// It might actually be true! This should be considered undefined
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
private:
	template<typename T>
	static T && error_out_of_range(T && value) noexcept {
		return std::forward<T>(value);
	}
};

}	// namespace policy_detail

using null_policy = basic_policy<policy_detail::null_policy>;

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
