// Determine if a type is an iterator
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

#include <containers/has_nested_type.hpp>

namespace containers {
namespace detail {

CONTAINERS_MAKE_NESTED_TYPE_TEST(iterator_category)

}	// namespace detail

template<typename Iterator>
constexpr auto is_iterator = detail::has_nested_type_iterator_category<Iterator>;

}	// namespace containers
