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

#include <functional>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename InputIterator, typename Initial>
using integer_accumulate_t = decltype(
	std::declval<Initial>() +
	(
		*std::declval<InputIterator>() *
		std::declval<bounded::integer<
			0,
			static_cast<intmax_t>(std::numeric_limits<typename std::iterator_traits<InputIterator>::difference_type>::max())
		>>()
	)
);

template<typename InputIterator, typename Initial, typename BinaryFunction>
using accumulate_t = std::conditional_t<
	bounded::is_bounded_integer<Initial> and std::is_same<BinaryFunction, std::plus<>>::value,
	integer_accumulate_t<InputIterator, Initial>,
	std::decay_t<Initial>
>;

}	// namespace detail



template<typename Result, typename InputIterator, typename Sentinel, typename Initial, typename BinaryFunction>
constexpr auto accumulate(InputIterator first, Sentinel const last, Initial && initial, BinaryFunction function) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(function(std::move(std::declval<Result>()), *first)) and std::is_nothrow_move_constructible<std::decay_t<Result>>::value) {
	auto result = static_cast<Result>(std::forward<Initial>(initial));
	for (; first != last; ++first) {
		result = static_cast<Result>(function(std::move(result), *first));
	}
	return result;
}


template<
	typename InputIterator,
	typename Sentinel,
	typename Initial,
	typename BinaryFunction
>
constexpr auto accumulate(InputIterator first, Sentinel const last, Initial && initial, BinaryFunction function) BOUNDED_NOEXCEPT(
	::containers::accumulate<detail::accumulate_t<InputIterator, Initial, BinaryFunction>>(first, last, std::forward<Initial>(initial), std::move(function))
)


template<typename Result, typename InputIterator, typename Sentinel, typename Initial>
constexpr auto accumulate(InputIterator first, Sentinel const last, Initial && initial) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(first, last, std::forward<Initial>(initial), std::plus<>{})
)

template<typename InputIterator, typename Sentinel, typename Initial>
constexpr auto accumulate(InputIterator first, Sentinel const last, Initial && initial) BOUNDED_NOEXCEPT(
	::containers::accumulate(first, last, std::forward<Initial>(initial), std::plus<>{})
)


template<typename Result, typename InputIterator, typename Sentinel>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(first, last, bounded::constant<0>, std::plus<>{})
)

template<typename InputIterator, typename Sentinel>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate(first, last, bounded::constant<0>, std::plus<>{})
)

}	// namespace containers
