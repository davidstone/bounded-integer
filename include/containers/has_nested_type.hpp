// Determines if a type has a nested type
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

#include <type_traits>

#define CONTAINERS_MAKE_NESTED_TYPE_TEST(type) \
\
template<typename T> \
constexpr auto has_nested_type_impl_ ## type(typename std::remove_reference_t<T>::type *) noexcept { return true; } \
\
template<typename> \
constexpr auto has_nested_type_impl_ ## type(...) noexcept { return false; } \
\
template<typename T> \
constexpr auto has_nested_type_ ## type = detail::has_nested_type_impl_ ##type<T>(nullptr);

