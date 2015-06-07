// result_type of arithmetic operations
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

#include "../../noexcept.hpp"

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename Min, typename Max>
struct min_max_t {
	template<typename Minimum, typename Maximum>
	constexpr min_max_t(Minimum && min_, Maximum && max_) noexcept(noexcept(std::is_nothrow_constructible<Min, Minimum &&>::value and std::is_nothrow_constructible<Max, Maximum &&>::value)):
		min(std::forward<Minimum>(min_)),
		max(std::forward<Maximum>(max_)) {
	}
	Min min;
	Max max;
};

template<typename Min, typename Max>
constexpr auto min_max(Min && min, Max && max) BOUNDED_NOEXCEPT(
	(min_max_t<std::remove_cv_t<std::remove_reference_t<Min>>, std::remove_cv_t<std::remove_reference_t<Max>>>(std::forward<Min>(min), std::forward<Max>(max)))
)

}	// namespace detail
}	// namespace bounded

