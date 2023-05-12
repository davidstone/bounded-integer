// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.append_after;

import containers.algorithms.copy_or_relocate_from;
import containers.c_array;
import containers.is_range;
import containers.iterator_t;
import containers.range_value_t;
import containers.supports_lazy_insert_after;

import std_module;

namespace containers {

template<typename Target, typename Source>
constexpr auto append_after_impl(Target & target, iterator_t<Target const &> before_position, Source && source) -> void {
	::containers::copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
		before_position = target.lazy_insert_after(before_position, make);
	});
}

export template<supports_lazy_insert_after Target>
constexpr auto append_after(Target & target, iterator_t<Target const &> before_position, range auto && source) -> void {
	::containers::append_after_impl(target, before_position, OPERATORS_FORWARD(source));
}

export template<supports_lazy_insert_after Target>
constexpr auto append_after(Target &, iterator_t<Target const &>, empty_c_array_parameter) -> void {
}
export template<supports_lazy_insert_after Target, std::size_t init_size>
constexpr auto append_after(Target & target, iterator_t<Target const &> before_position, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::append_after_impl(target, before_position, std::move(source));
}

} // namespace containers
