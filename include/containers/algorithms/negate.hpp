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

#include <type_traits>

namespace containers {
namespace detail {

template<typename Predicate>
struct negate_t {
	constexpr negate_t(Predicate && predicate) noexcept(std::is_nothrow_move_constructible<Predicate>::value):
		m_predicate(std::move(predicate))
	{
	}
	template<typename... Args>
	constexpr decltype(auto) operator()(Args && ... args) BOUNDED_NOEXCEPT(
		!m_predicate(std::forward<Args>(args)...)
	)
private:
	Predicate m_predicate;
};

}	// namespace detail

template<typename Predicate>
constexpr auto negate(Predicate predicate) BOUNDED_NOEXCEPT(
	detail::negate_t<Predicate>(std::move(predicate))
)

}	// namespace containers
