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

#include <containers/is_iterator.hpp>
#include <containers/type_list.hpp>

namespace containers {
namespace detail {

template<typename LHS, typename RHS, typename Enable = void>
struct comparable_c : std::false_type {};

template<typename LHS, typename RHS>
struct comparable_c<
	LHS,
	RHS,
	void_t<
		decltype(std::declval<LHS const &>() == std::declval<RHS const &>()),
		decltype(std::declval<LHS const &>() != std::declval<RHS const &>())
	>
> : std::true_type {};

template<typename LHS, typename RHS>
constexpr auto comparable = comparable_c<LHS, RHS>::value;

}	// namespace detail

template<typename Iterator, typename Sentinel>
constexpr auto is_iterator_sentinel = is_iterator<Iterator> and detail::comparable<Iterator, Sentinel>;

}	// namespace containers
