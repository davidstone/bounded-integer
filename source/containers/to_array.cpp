// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.to_array;

import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.is_container;
import containers.maximum_array_size;
import containers.range_value_t;
import containers.size;
import containers.sized_range;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename T, typename Source>
constexpr auto to_array_impl(Source && source) {
	constexpr auto size = bounded::constant<containers::size(source)>;
	if constexpr (size == 0_bi) {
		return array<T, size>();
	} else {
		auto [...indexes] = bounded::index_sequence_struct<static_cast<std::size_t>(size - 1_bi)>();
		auto it = containers::begin(OPERATORS_FORWARD(source));
		if constexpr (is_container<Source>) {
			return array<T, size>({std::move(*it), (static_cast<void>(indexes), std::move(*++it))...});
		} else {
			return array<T, size>({*it, (static_cast<void>(indexes), *++it)...});
		}
	}
}
export template<typename T, statically_sized_range Range>
constexpr auto to_array(Range && source) {
	return to_array_impl<T>(OPERATORS_FORWARD(source));
}
export template<statically_sized_range Range>
constexpr auto to_array(Range && source) {
	return to_array_impl<range_value_t<Range>>(OPERATORS_FORWARD(source));
}
export template<typename T, std::size_t size>
constexpr auto to_array(c_array<T, size> && source) {
	return to_array_impl<T>(std::move(source));
}
export template<typename T>
constexpr auto to_array(empty_c_array_parameter) {
	return array<T, 0_bi>();
}

export template<auto size_>
constexpr auto make_array_n(bounded::constant_t<size_> size, auto && value) {
	using result_t = array<std::decay_t<decltype(value)>, size>;
	if constexpr (size == 0_bi) {
		return result_t();
	} else {
		auto [...indexes] = bounded::index_sequence_struct<size_ - 1>();
		// Use the comma operator to expand the variadic pack. Move the last
		// element if possible. Order of evaluation is well-defined for
		// aggregate initialization, so there is no risk of copy-after-move
		return result_t({(void(indexes), value)..., OPERATORS_FORWARD(value)});
	}
}

} // namespace containers
