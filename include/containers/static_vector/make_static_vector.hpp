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

#include <containers/static_vector/static_vector.hpp>

#include <limits>
#include <type_traits>

namespace containers {

template<typename T, typename Size>
constexpr auto make_static_vector(Size const size) BOUNDED_NOEXCEPT(
	static_vector<T, static_cast<std::intmax_t>(std::numeric_limits<Size>::max())>(size)
)
template<typename T, typename Size>
constexpr auto make_static_vector(Size const size, T const & value) BOUNDED_NOEXCEPT(
	static_vector<std::decay_t<T>, static_cast<std::intmax_t>(std::numeric_limits<Size>::max())>(size, value)
)

}	// namespace containers
