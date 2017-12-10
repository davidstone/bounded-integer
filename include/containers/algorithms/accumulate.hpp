// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename InputIterator, typename Initial, typename BinaryFunction, bool is_bounded = bounded::is_bounded_integer<Initial>>
struct accumulate_c {
	using type = Initial;
};

template<typename InputIterator, typename Initial>
struct accumulate_c<InputIterator, Initial, std::plus<>, true> {
	using type = decltype(
		std::declval<Initial>() +
		(
			*std::declval<InputIterator>() *
			std::declval<bounded::integer<
				0,
				static_cast<intmax_t>(std::numeric_limits<typename std::iterator_traits<InputIterator>::difference_type>::max())
			>>()
		)
	);
};

template<typename InputIterator, typename Initial, typename BinaryFunction>
using accumulate_t = typename accumulate_c<InputIterator, std::decay_t<Initial>, BinaryFunction>::type;

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


template<
	typename Result,
	typename InputIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(bounded::is_bounded_integer<typename InputIterator::value_type>)
>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(first, last, bounded::constant<0>, std::plus<>{})
)

template<
	typename InputIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(bounded::is_bounded_integer<typename InputIterator::value_type>)
>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate(first, last, bounded::constant<0>, std::plus<>{})
)


template<
	typename Result,
	typename InputIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(!bounded::is_bounded_integer<typename InputIterator::value_type>)
>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(first, last, typename InputIterator::value_type{}, std::plus<>{})
)

template<
	typename InputIterator,
	typename Sentinel,
	BOUNDED_REQUIRES(!bounded::is_bounded_integer<typename InputIterator::value_type>)
>
constexpr auto accumulate(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	::containers::accumulate(first, last, typename InputIterator::value_type{}, std::plus<>{})
)

}	// namespace containers
