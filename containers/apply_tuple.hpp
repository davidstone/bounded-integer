// Apply a function to all values in a tuple
// Copyright (C) 2013 David Stone
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

namespace containers {

template<std::size_t...>
class index_list { };

// I can't seem to get enable_if_t to work with partial specializations. You 
// also cannot specialize template aliases. I could bypass both of these
// problems with std::conditional, but this way allows me to essentially define
// temporary expressions / types using template arguments, and thus the
// implementation is a little cleaner. It's also a bit harder to get at the
// variadic std::size_t pack.
template<std::size_t index, std::size_t size, typename Indexes = index_list<>, typename Enable = void>
class make_index {
public:
	using type = Indexes;
};
template<std::size_t index, std::size_t size, std::size_t... lower_indexes>
class make_index<index, size, index_list<lower_indexes...>, typename std::enable_if<index < size, void>::type> {
public:
	using type = typename make_index<index + 1, size, index_list<lower_indexes..., index>>::type;
};

template<typename Tuple>
using tuple_indexes_t = typename make_index<0, std::tuple_size<Tuple>::value>::type;

// I should probably add a conditional noexcept specification to these
template<typename Function, typename Tuple, std::size_t... indexes>
constexpr auto apply_helper(Function const & f, Tuple && tuple_args, index_list<indexes...>) ->
	decltype(f(std::get<indexes>(std::forward<Tuple>(tuple_args))...)) {
	return f(std::get<indexes>(std::forward<Tuple>(tuple_args))...);
}

template<typename Function, typename Tuple, typename Indexes = tuple_indexes_t<Tuple>>
constexpr auto apply(Function const & f, Tuple && tuple_args) ->
	decltype(apply_helper(f, std::forward<Tuple>(tuple_args), Indexes{})) {
	return apply_helper(f, std::forward<Tuple>(tuple_args), Indexes{});
}

}	// namespace containers
#endif	// CONTAINERS_APPLY_TUPLE_HPP_
