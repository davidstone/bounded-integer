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

#include <bounded_integer/bounded_integer.hpp>

namespace containers {
namespace detail {

template<typename T>
constexpr auto has_member_addressof(std::remove_reference_t<decltype(std::declval<T &>().operator&())> *) noexcept { return true; }
template<typename T>
constexpr auto has_member_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto has_free_addressof(std::remove_reference_t<decltype(operator&(std::declval<T &>()))> *) noexcept { return true; }
template<typename T>
constexpr auto has_free_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto has_addressof(std::remove_reference_t<decltype(&(std::declval<T &>()))> *) noexcept { return true; }
template<typename T>
constexpr auto has_addressof(...) noexcept { return false; }

template<typename T>
constexpr auto constexpr_addressof =
	!has_member_addressof<T>(nullptr) and
	!has_free_addressof<T>(nullptr) and
	has_addressof<T>(nullptr);

}	// namespace detail

template<typename T, BOUNDED_REQUIRES(detail::constexpr_addressof<T>)>
constexpr auto addressof(T & t) noexcept {
	return &t;
}

// TODO: Use compiler built-ins as a fallback
template<typename T, BOUNDED_REQUIRES(!detail::constexpr_addressof<T>)>
auto addressof(T & t) noexcept {
	return std::addressof(t);
}

}	// namespace containers
