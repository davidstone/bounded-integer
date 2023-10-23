// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.assign_to_empty;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.uninitialized;
import containers.assign_to_empty_into_capacity;
import containers.begin_end;
import containers.c_array;
import containers.can_set_size;
import containers.get_source_size;
import containers.initializer_range;
import containers.is_empty;
import containers.lazy_push_back;
import containers.member_assign;
import containers.range_value_t;
import containers.reservable;
import containers.resizable_container;
import containers.size;
import containers.size_then_use_range;
import containers.sized_range;
import containers.supports_lazy_insert_after;
import containers.take;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

template<typename Target, typename Source>
concept size_always_fits_in_capacity =
	numeric_traits::max_value<range_size_t<Source>> <= numeric_traits::min_value<decltype(bounded::declval<Target>().capacity())>;

template<typename Target>
concept has_replace_empty_allocation = requires(Target & target, containers::range_size_t<Target> const source_size) {
	target.replace_empty_allocation(source_size);
};

template<typename Target>
constexpr auto do_reserve(Target & target, auto const source_size) -> void {
	if constexpr (has_replace_empty_allocation<Target>) {
		target.replace_empty_allocation(source_size);
	} else {
		auto temp = Target();
		temp.reserve(source_size);
		target = std::move(temp);
	}
}

template<typename Target, typename Source>
constexpr auto assign_to_empty_impl(Target & target, Source && source) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
	if constexpr (size_always_fits_in_capacity<Target, Source>) {
		assign_to_empty_into_capacity(target, OPERATORS_FORWARD(source));
	} else if constexpr (can_set_size<Target> and reservable<Target> and size_then_use_range<Source>) {
		auto const source_size = ::bounded::assume_in_range<range_size_t<Target>>(::containers::get_source_size<Target>(source));
		if (target.capacity() < source_size) {
			::containers::do_reserve(target, source_size);
		}
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), containers::begin(target));
		target.set_size(source_size);
	} else if constexpr (can_set_size<Target> and !reservable<Target>) {
		auto const target_position = containers::begin(target);
		auto const new_end = containers::uninitialized_copy_no_overlap(::containers::check_size_not_greater_than(OPERATORS_FORWARD(source), target.capacity()), target_position);
		auto const source_size = [&] {
			if constexpr (sized_range<Source>) {
				return ::containers::get_source_size<Target>(source);
			} else {
				return ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
			}
		}();
		target.set_size(source_size);
	} else if constexpr (lazy_push_backable<Target>) {
		copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
			::containers::lazy_push_back(target, make);
		});
	} else if constexpr (supports_lazy_insert_after<Target>) {
		copy_or_relocate_from(OPERATORS_FORWARD(source), [&target, it = target.before_begin()](auto make) mutable {
			it = target.lazy_insert_after(it, make);
		});
	} else {
		::containers::member_assign(target, OPERATORS_FORWARD(source));
	}
}

template<typename Container>
concept assignable_to_empty = resizable_container<Container> or supports_lazy_insert_after<Container>;

export template<assignable_to_empty Target, initializer_range<Target> Source>
constexpr auto assign_to_empty(Target & target, Source && source) -> void {
	::containers::assign_to_empty_impl(target, OPERATORS_FORWARD(source));
}

export constexpr auto assign_to_empty(assignable_to_empty auto & target, empty_c_array_parameter) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
}
export template<assignable_to_empty Target, std::size_t init_size>
	requires std::is_move_constructible_v<range_value_t<Target>>
constexpr auto assign_to_empty(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::assign_to_empty_impl(target, std::move(source));
}

} // namespace containers
