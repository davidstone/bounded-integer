// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/returns.hpp>
#include <operators/compound_assignment.hpp>

#include "../../test_assert.hpp"

export module bounded.arithmetic.increment_decrement;

import bounded.arithmetic.plus;
import bounded.arithmetic.unary_minus;
import bounded.integer;

import operators;
import std_module;

namespace bounded {

OPERATORS_IMPORT_COMPOUND_ASSIGNMENT(export)
export using operators::binary::operator-; \

template<typename T>
concept incrementable_by_bounded = requires(T value) {
	value += constant<1>;
};
template<typename T>
concept decrementable_by_bounded = requires(T value) {
	value -= constant<1>;
};

inline namespace arithmetic {

export constexpr auto operator++(incrementable_by_bounded auto & value) -> decltype(auto) {
	return value += constant<1>;
}

export constexpr auto operator--(decrementable_by_bounded auto & value) -> decltype(auto) {
	return value -= constant<1>;
}

} // namespace arithmetic
} // namespace bounded

static_assert([]{
	auto x = bounded::integer<0, 1>(bounded::constant<0>);
	auto & result = ++x;
	BOUNDED_TEST(std::addressof(result) == std::addressof(x));
	BOUNDED_TEST(x == bounded::constant<1>);
	return true;
}());

static_assert([]{
	auto x = bounded::integer<0, 1>(bounded::constant<1>);
	auto & result = --x;
	BOUNDED_TEST(std::addressof(result) == std::addressof(x));
	BOUNDED_TEST(x == bounded::constant<0>);
	return true;
}());
