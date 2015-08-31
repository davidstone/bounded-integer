// Apply a function to all values in a tuple
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

#include <tuple>
#include <type_traits>
#include <utility>

namespace containers {

// I should probably add a conditional noexcept specification to these
template<typename Function, typename Tuple, std::size_t... indexes>
constexpr decltype(auto) apply_helper(Function && f, Tuple && tuple_args, std::index_sequence<indexes...>) {
	return std::forward<Function>(f)(std::get<indexes>(std::forward<Tuple>(tuple_args))...);
}

template<typename Function, typename Tuple>
constexpr decltype(auto) apply(Function && f, Tuple && tuple_args) {
	using indexes = std::make_index_sequence<std::tuple_size<Tuple>::value>;
	return apply_helper(std::forward<Function>(f), std::forward<Tuple>(tuple_args), indexes{});
}

}	// namespace containers
