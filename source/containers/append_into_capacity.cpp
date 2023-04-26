// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.append_into_capacity;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.c_array;
import containers.can_set_size;
import containers.get_source_size;
import containers.is_range;
import containers.lazy_push_back_into_capacity;
import containers.legacy_append;
import containers.push_back_into_capacity;
import containers.range_value_t;
import containers.size;
import containers.size_then_use_range;

import bounded;
import std_module;

namespace containers {

template<typename Target, typename Source>
constexpr auto append_into_capacity_impl(Target & target, Source && source) -> void {
	if constexpr (can_set_size<Target>) {
		auto const original_size = ::containers::size(target);
		auto const target_position = containers::end(target);
		if constexpr (size_then_use_range<Source>) {
			auto const source_size = ::containers::get_source_size<Target>(source);
			containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
			target.set_size(original_size + source_size);
		} else {
			// TODO: Use an iterator that includes a count if we do not have a
			// random-access iterator for the target
			auto const new_end = containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
			auto const source_size = ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
			target.set_size(original_size + source_size);
		}
	} else if constexpr (can_lazy_push_back_into_capacity<Target>) {
		::containers::copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
			::containers::lazy_push_back_into_capacity(target, make);
		});
	} else {
		::containers::legacy_append(target, OPERATORS_FORWARD(source));
	}
}

export constexpr auto append_into_capacity(can_push_back_into_capacity auto & target, range auto && source) -> void {
	::containers::append_into_capacity_impl(target, OPERATORS_FORWARD(source));
}

export template<can_push_back_into_capacity Target>
constexpr auto append_into_capacity(Target &, empty_c_array_parameter) -> void {
}
export template<can_push_back_into_capacity Target, std::size_t init_size>
constexpr auto append_into_capacity(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::append_into_capacity_impl(target, std::move(source));
}

} // namespace containers
