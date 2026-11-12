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
import containers.index_type;
import containers.is_container;
import containers.maximum_array_size;
import containers.range_value_t;
import containers.size;
import containers.sized_range;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename T>
constexpr auto make_index(auto const index) {
	if constexpr (std::is_array_v<std::remove_cvref_t<T>>) {
		return std::size_t(index);
	} else {
		return containers::index_type<T>(index);
	}
}

template<typename T, typename Source>
constexpr auto to_array_impl(Source && source) {
	constexpr auto size = bounded::constant<containers::size(source)>;
	static constexpr auto const [...indexes] = bounded::index_sequence_struct(size);
	return array<T, size>({OPERATORS_FORWARD(source)[containers::make_index<Source>(indexes)]...});
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
