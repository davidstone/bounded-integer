// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.assign_to_empty_into_capacity;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.c_array;
import containers.can_set_size;
import containers.member_assign;

import containers.get_source_size;
import containers.initializer_range;
import containers.is_empty;
import containers.lazy_push_back;
import containers.range_value_t;
import containers.resizable_container;
import containers.size;
import containers.size_then_use_range;
import containers.supports_lazy_insert_after;

import bounded;
import std_module;

namespace containers {

template<typename Target, typename Source>
constexpr auto assign_to_empty_into_capacity_impl(Target & target, Source && source) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
	if constexpr (can_set_size<Target>) {
		auto const target_position = containers::begin(target);
		auto const new_end = containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
		auto const source_size = [&] {
			if constexpr (sized_range<Source &>) {
				return ::containers::get_source_size<Target>(source);
			} else {
				return ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
			}
		}();
		target.set_size(source_size);
	} else {
		::containers::member_assign(target, OPERATORS_FORWARD(source));
	}
}

template<typename Container>
concept assignable_to_empty = resizable_container<Container> or supports_lazy_insert_after<Container>;

export template<assignable_to_empty Target, initializer_range<Target> Source>
constexpr auto assign_to_empty_into_capacity(Target & target, Source && source) -> void {
	::containers::assign_to_empty_into_capacity_impl(target, OPERATORS_FORWARD(source));
}

export constexpr auto assign_to_empty_into_capacity(assignable_to_empty auto & target, empty_c_array_parameter) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
}
export template<assignable_to_empty Target, std::size_t init_size>
	requires std::is_move_constructible_v<range_value_t<Target>>
constexpr auto assign_to_empty_into_capacity(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::assign_to_empty_into_capacity_impl(target, std::move(source));
}

} // namespace containers
