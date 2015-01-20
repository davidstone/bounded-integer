// Apply a function to all values in a tuple
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

#ifndef CONTAINERS_APPLY_TUPLE_HPP_
#define CONTAINERS_APPLY_TUPLE_HPP_

#include <type_traits>
#include <utility>

namespace containers {

// I should probably add a conditional noexcept specification to these
template<typename Function, typename Tuple, std::size_t... indexes>
constexpr auto apply_helper(Function const & f, Tuple && tuple_args, std::index_sequence<indexes...>) ->
	decltype(f(std::get<indexes>(std::forward<Tuple>(tuple_args))...)) {
	return f(std::get<indexes>(std::forward<Tuple>(tuple_args))...);
}

template<typename Function, typename Tuple, typename Indexes = std::make_index_sequence<std::tuple_size<Tuple>::value>>
constexpr auto apply(Function const & f, Tuple && tuple_args) ->
	decltype(apply_helper(f, std::forward<Tuple>(tuple_args), Indexes{})) {
	return apply_helper(f, std::forward<Tuple>(tuple_args), Indexes{});
}

}	// namespace containers
#endif	// CONTAINERS_APPLY_TUPLE_HPP_
