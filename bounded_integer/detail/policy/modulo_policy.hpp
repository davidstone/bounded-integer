// Also known as wrapping
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

#ifndef BOUNDED_INTEGER_POLICY_MODULO_POLICY_HPP_
#define BOUNDED_INTEGER_POLICY_MODULO_POLICY_HPP_

#include "basic_policy.hpp"
#include "../class.hpp"
#include "../common_type.hpp"
#include "../operators/arithmetic.hpp"
#include <type_traits>

namespace bounded {
namespace policy_detail {

class modulo_policy {
private:
	template<intmax_t value>
	static constexpr auto make() noexcept -> integer<value, value> {
		return { value, non_check };
	}
	template<typename T>
	static constexpr auto make(T && value) noexcept -> integer<detail::basic_numeric_limits<T>::min(), detail::basic_numeric_limits<T>::max()> {
		return { std::forward<T>(value), non_check };
	}
public:
	template<typename T, typename Minimum, typename Maximum>
	static constexpr auto assignment(T && value, Minimum && minimum, Maximum && maximum) noexcept {
		static_assert(is_bounded_integer<std::decay_t<Minimum>>::value, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<std::decay_t<Maximum>>::value, "Only bounded::integer types are supported.");
		return positive_remainder(
			(value - minimum) % (maximum - minimum + make<1>()),
			maximum - minimum + make<1>()
		) + minimum;
	}

	static constexpr bool is_modulo = true;
	static constexpr bool overflow_is_error = false;
	
private:
	template<typename T, typename Size>
	static constexpr auto positive_remainder(T && value, Size && size) noexcept {
		using result_type = std::common_type_t<std::decay_t<T>, std::decay_t<decltype(std::forward<T>(value) + std::forward<Size>(size))>>;
		return (value < make<0>()) ?
			result_type(std::forward<T>(value) + std::forward<Size>(size), non_check) :
			result_type(std::forward<T>(value), non_check)
		;
	}
};

}	// namespace policy_detail

using modulo_policy = basic_policy<policy_detail::modulo_policy>;

}	// namespace bounded
#endif	// BOUNDED_INTEGER_POLICY_MODULO_POLICY_HPP_
