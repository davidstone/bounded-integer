// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Range, typename Initial, typename BinaryFunction, bool is_bounded = bounded::bounded_integer<Initial>>
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
					bounded::max_value<
						typename std::iterator_traits<decltype(begin(std::declval<Range>()))>::difference_type
					>.value()
				>
			>>()
		)
	);
};

template<typename Range, typename Initial, typename BinaryFunction>
using accumulate_t = typename accumulate_c<Range, std::decay_t<Initial>, BinaryFunction>::type;

}	// namespace detail



template<typename Result, range Range, typename Initial, typename BinaryFunction>
constexpr auto accumulate(Range && source, Initial && initial, BinaryFunction function) {
	auto result = static_cast<Result>(BOUNDED_FORWARD(initial));
	for (decltype(auto) value : BOUNDED_FORWARD(source)) {
		result = static_cast<Result>(function(std::move(result), BOUNDED_FORWARD(value)));
	}
	return result;
}


template<range Range, typename Initial, typename BinaryFunction>
constexpr auto accumulate(Range && source, Initial && initial, BinaryFunction function) {
	return ::containers::accumulate<detail::accumulate_t<Range, Initial, BinaryFunction>>(
		BOUNDED_FORWARD(source),
		BOUNDED_FORWARD(initial),
		std::move(function)
	);
}


template<typename Result>
constexpr auto accumulate(range auto && source, auto && initial) {
	return ::containers::accumulate<Result>(BOUNDED_FORWARD(source), BOUNDED_FORWARD(initial), std::plus<>{});
}

template<typename Initial>
constexpr auto accumulate(range auto && source, Initial && initial) {
	return ::containers::accumulate(BOUNDED_FORWARD(source), BOUNDED_FORWARD(initial), std::plus<>{});
}


namespace detail {

template<typename T> requires std::is_default_constructible_v<T>
constexpr auto initial_accumulate_value() {
	if constexpr (bounded::bounded_integer<T>) {
		return 0_bi;
	} else {
		return T{};
	}
}

}	// namespace detail

template<typename Result, range Range>
constexpr auto accumulate(Range && source) {
	return ::containers::accumulate<Result>(
		BOUNDED_FORWARD(source),
		detail::initial_accumulate_value<typename std::remove_reference_t<Range>::value_type>(),
		std::plus<>{}
	);
}

template<range Range>
constexpr auto accumulate(Range && source) {
	return ::containers::accumulate(
		BOUNDED_FORWARD(source),
		detail::initial_accumulate_value<typename std::remove_reference_t<Range>::value_type>(),
		std::plus<>{}
	);
}

}	// namespace containers
