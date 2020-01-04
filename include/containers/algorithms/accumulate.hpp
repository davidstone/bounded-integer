// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Range, typename Initial, typename BinaryFunction>
struct accumulate_c {
	using type = Initial;
};

template<typename Range, typename Initial> requires bounded::bounded_integer<Initial>
struct accumulate_c<Range, Initial, std::plus<>> {
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



template<typename Result>
constexpr auto accumulate(range auto && source, auto && initial, auto function) {
	auto result = static_cast<Result>(OPERATORS_FORWARD(initial));
	for (decltype(auto) value : OPERATORS_FORWARD(source)) {
		result = static_cast<Result>(function(std::move(result), OPERATORS_FORWARD(value)));
	}
	return result;
}


constexpr auto accumulate(range auto && source, auto && initial, auto function) {
	return ::containers::accumulate<detail::accumulate_t<decltype(source), decltype(initial), decltype(function)>>(
		OPERATORS_FORWARD(source),
		OPERATORS_FORWARD(initial),
		std::move(function)
	);
}


template<typename Result>
constexpr auto accumulate(range auto && source, auto && initial) {
	return ::containers::accumulate<Result>(OPERATORS_FORWARD(source), OPERATORS_FORWARD(initial), std::plus<>{});
}

constexpr auto accumulate(range auto && source, auto && initial) {
	return ::containers::accumulate(OPERATORS_FORWARD(source), OPERATORS_FORWARD(initial), std::plus<>{});
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

template<typename Result>
constexpr auto accumulate(range auto && source) {
	return ::containers::accumulate<Result>(
		OPERATORS_FORWARD(source),
		detail::initial_accumulate_value<typename std::remove_reference_t<decltype(source)>::value_type>(),
		std::plus<>{}
	);
}

constexpr auto accumulate(range auto && source) {
	return ::containers::accumulate(
		OPERATORS_FORWARD(source),
		detail::initial_accumulate_value<typename std::remove_reference_t<decltype(source)>::value_type>(),
		std::plus<>{}
	);
}

}	// namespace containers
