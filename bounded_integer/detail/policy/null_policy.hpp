// Does nothing. Overflow is undefined behavior
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

#ifndef BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_

#include "basic_policy.hpp"
#include "common_policy.hpp"
#include "../is_bounded_integer.hpp"
#include "../operators/comparison.hpp"
#include <utility>

namespace bounded {
namespace policy_detail {

struct null_policy {
	// The identity function is intentionally not constexpr. This provides
	// compile-time checking if used in a constexpr context. If this is called
	// at run-time, the optimizer should detect that all branches return the
	// same value and eliminate all branching, creating no overhead. See
	// http://stackoverflow.com/questions/20461121/constexpr-error-at-compile-time-but-no-overhead-at-run-time
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept -> T && {
		static_assert(is_bounded_integer<std::decay_t<Minimum>>::value, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<std::decay_t<Maximum>>::value, "Only bounded::integer types are supported.");
		return (minimum <= value and value <= maximum) ?
			std::forward<T>(value) :
			error_out_of_range(std::forward<T>(value));
	}

	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = true;
private:
	template<typename T>
	static auto error_out_of_range(T && value) noexcept -> T && {
		return std::forward<T>(value);
	}
};

}	// namespace policy_detail

using null_policy = basic_policy<policy_detail::null_policy>;



template<typename policy>
struct common_policy<policy, null_policy> {
	using type = policy;
};
template<typename policy>
struct common_policy<null_policy, policy> {
	using type = policy;
};
template<>
struct common_policy<null_policy, null_policy> {
	using type = null_policy;
};

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_NULL_POLICY_HPP_
