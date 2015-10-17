// Also known as wrapping
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

#include "../class.hpp"
#include "../common_type.hpp"
#include "../comparison.hpp"
#include "../arithmetic/minus.hpp"
#include "../arithmetic/modulus.hpp"
#include "../arithmetic/plus.hpp"
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
		static_assert(is_bounded_integer<std::decay_t<Minimum>>, "Only bounded::integer types are supported.");
		static_assert(is_bounded_integer<std::decay_t<Maximum>>, "Only bounded::integer types are supported.");
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
