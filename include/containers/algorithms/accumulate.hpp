// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/noexcept_iterable.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Range, typename Initial, typename BinaryFunction, bool is_bounded = bounded::is_bounded_integer<Initial>>
struct accumulate_c {
	using type = Initial;
};

template<typename Range, typename Initial>
struct accumulate_c<Range, Initial, std::plus<>, true> {
	using type = decltype(
		std::declval<Initial>() +
		(
			std::declval<typename std::remove_reference_t<Range>::value_type>() *
			std::declval<bounded::integer<
				0,
				bounded::detail::normalize<
					std::numeric_limits<
						typename std::iterator_traits<decltype(begin(std::declval<Range>()))>::difference_type
					>::max().value()
				>
			>>()
		)
	);
};

template<typename Range, typename Initial, typename BinaryFunction>
using accumulate_t = typename accumulate_c<Range, std::decay_t<Initial>, BinaryFunction>::type;

}	// namespace detail



template<typename Result, typename Range, typename Initial, typename BinaryFunction> requires is_range<Range>
constexpr auto accumulate(Range && range, Initial && initial, BinaryFunction function) noexcept(
	detail::noexcept_iterable<Range> and
	noexcept(function(std::move(std::declval<Result>()), *begin(BOUNDED_FORWARD(range)))) and
	std::is_nothrow_move_constructible_v<std::decay_t<Result>>
) {
	auto result = static_cast<Result>(BOUNDED_FORWARD(initial));
	for (decltype(auto) value : BOUNDED_FORWARD(range)) {
		result = static_cast<Result>(function(std::move(result), BOUNDED_FORWARD(value)));
	}
	return result;
}


template<typename Range, typename Initial, typename BinaryFunction> requires is_range<Range>
constexpr auto accumulate(Range && range, Initial && initial, BinaryFunction function) BOUNDED_NOEXCEPT(
	::containers::accumulate<detail::accumulate_t<Range, Initial, BinaryFunction>>(
		BOUNDED_FORWARD(range),
		BOUNDED_FORWARD(initial),
		std::move(function)
	)
)


template<typename Result, typename Range, typename Initial> requires is_range<Range>
constexpr auto accumulate(Range && range, Initial && initial) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(BOUNDED_FORWARD(range), BOUNDED_FORWARD(initial), std::plus<>{})
)

template<typename Range, typename Initial> requires is_range<Range>
constexpr auto accumulate(Range && range, Initial && initial) BOUNDED_NOEXCEPT(
	::containers::accumulate(BOUNDED_FORWARD(range), BOUNDED_FORWARD(initial), std::plus<>{})
)


namespace detail {

template<typename T> requires bounded::is_bounded_integer<T>
constexpr auto initial_accumulate_value() noexcept{
	return 0_bi;
}

template<typename T> requires (!bounded::is_bounded_integer<T>)
constexpr auto initial_accumulate_value() BOUNDED_NOEXCEPT_VALUE(
	T{}
)

}	// namespace detail

template<typename Result, typename Range> requires is_range<Range>
constexpr auto accumulate(Range && range) BOUNDED_NOEXCEPT(
	::containers::accumulate<Result>(
		BOUNDED_FORWARD(range),
		detail::initial_accumulate_value<typename std::remove_reference_t<Range>::value_type>(),
		std::plus<>{}
	)
)

template<typename Range> requires is_range<Range>
constexpr auto accumulate(Range && range) BOUNDED_NOEXCEPT(
	::containers::accumulate(
		BOUNDED_FORWARD(range),
		detail::initial_accumulate_value<typename std::remove_reference_t<Range>::value_type>(),
		std::plus<>{}
	)
)

}	// namespace containers
