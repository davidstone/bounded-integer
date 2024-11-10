// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.accumulate;

import containers.array;
import containers.begin_end;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.range;
import containers.range_value_t;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Range, typename Initial, typename BinaryFunction>
struct accumulate_c {
	using type = Initial;
};

template<typename Range, typename Initial> requires bounded::bounded_integer<Initial>
struct accumulate_c<Range, Initial, std::plus<>> {
	using type = decltype(
		bounded::declval<Initial>() +
		(
			bounded::declval<containers::range_value_t<Range>>() *
			bounded::declval<bounded::integer<
				0,
				bounded::builtin_max_value<iter_difference_t<iterator_t<Range>>>
			>>()
		)
	);
};

template<typename Range, typename Initial> requires bounded::bounded_integer<Initial>
struct accumulate_c<Range, Initial, std::multiplies<>> {
	using type = decltype(
		bounded::declval<Initial>() *
		::bounded::pow(
			bounded::declval<range_value_t<Range>>(),
			bounded::declval<bounded::integer<
				0,
				bounded::builtin_max_value<iter_difference_t<iterator_t<Range>>>
			>>()
		)
	);
};

template<typename Range, typename Initial, typename BinaryFunction>
using accumulate_t = typename accumulate_c<Range, std::decay_t<Initial>, BinaryFunction>::type;


export template<typename Result>
constexpr auto accumulate(range auto && source, auto && initial, auto function) {
	auto result = static_cast<Result>(OPERATORS_FORWARD(initial));
	for (decltype(auto) value : OPERATORS_FORWARD(source)) {
		// Not ideal to have this `if` here
		if constexpr (bounded::bounded_integer<Result>) {
			result = ::bounded::assume_in_range<Result>(function(std::move(result), OPERATORS_FORWARD(value)));
		} else {
			result = static_cast<Result>(function(std::move(result), OPERATORS_FORWARD(value)));
		}
	}
	return result;
}


export constexpr auto accumulate(range auto && source, auto && initial, auto function) {
	return ::containers::accumulate<accumulate_t<decltype(source), decltype(initial), decltype(function)>>(
		OPERATORS_FORWARD(source),
		OPERATORS_FORWARD(initial),
		std::move(function)
	);
}


template<typename T>
constexpr auto initial_sum_value() {
	if constexpr (bounded::bounded_integer<T>) {
		return 0_bi;
	} else {
		return T(0);
	}
}

template<typename T>
constexpr auto initial_product_value() {
	if constexpr (bounded::bounded_integer<T>) {
		return 1_bi;
	} else {
		return T(1);
	}
}

export template<typename Result>
constexpr auto sum(range auto && source) {
	return ::containers::accumulate<Result>(
		OPERATORS_FORWARD(source),
		initial_sum_value<range_value_t<decltype(source)>>(),
		std::plus()
	);
}

export constexpr auto sum(range auto && source) {
	return ::containers::accumulate(
		OPERATORS_FORWARD(source),
		initial_sum_value<range_value_t<decltype(source)>>(),
		std::plus()
	);
}

export template<typename Result>
constexpr auto product(range auto && source) {
	return ::containers::accumulate<Result>(
		OPERATORS_FORWARD(source),
		initial_product_value<range_value_t<decltype(source)>>(),
		std::multiplies()
	);
}

export constexpr auto product(range auto && source) {
	return ::containers::accumulate(
		OPERATORS_FORWARD(source),
		initial_product_value<range_value_t<decltype(source)>>(),
		std::multiplies()
	);
}

} // namespace containers

static_assert(containers::sum(containers::array{0_bi, 3_bi, 2_bi, 3_bi, 5_bi}) == (0_bi + 3_bi + 2_bi + 3_bi + 5_bi));
static_assert(containers::sum(containers::array({0.0, 1.0, 2.0})) == (0.0 + 1.0 + 2.0));
