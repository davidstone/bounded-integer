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
import containers.begin_end;
import containers.c_array;
import containers.can_set_size;
import containers.initializer_range;
import containers.is_empty;
import containers.is_range;
import containers.lazy_push_back;
import containers.range_value_t;
import containers.reserve_if_reservable;
import containers.resizable_container;
import containers.size;
import containers.size_then_use_range;
import containers.supports_lazy_insert_after;

import bounded;
import std_module;

namespace containers {

export template<typename Target, typename Source>
concept member_iterator_assignable = requires(Target & target, Source && source) {
	target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
};

export template<typename Target, typename Source>
concept member_range_assignable = requires(Target & target, Source && source) {
	target.assign(OPERATORS_FORWARD(source));
};

export constexpr auto member_assign = []<typename Target, typename Source>(Target & target, Source && source) {
	if constexpr (member_range_assignable<Target, Source>) {
		target.assign(OPERATORS_FORWARD(source));
	} else {
		static_assert(member_iterator_assignable<Target, Source>);
		// TODO: Reserve for sized forward input ranges
		target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
	}
};

export template<typename Target>
constexpr auto get_source_size(size_then_use_range auto const & source) {
	auto const value = containers::linear_size(source);
	if constexpr (bounded::bounded_integer<decltype(value)>) {
		return value;
	} else {
		return ::bounded::assume_in_range<range_size_t<Target>>(value);
	}
};

constexpr auto exact_reserve = []<typename Container>(Container & target, auto const requested_size) {
	target.reserve(::bounded::assume_in_range<range_size_t<Container>>(requested_size));
};

template<typename Target, typename Source>
constexpr auto assign_to_empty_impl(Target & target, Source && source) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
	if constexpr (resizable_container<Target>) {
		if constexpr (can_set_size<Target> and reservable<Target> and size_then_use_range<Source>) {
			// This `if` is technically unnecessary, but the code generation is
			// better with it here
			if (containers::is_empty(source)) {
				return;
			}
			auto const source_size = ::containers::get_source_size<Target>(source);
			exact_reserve(target, source_size);
			containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), containers::begin(target));
			target.set_size(source_size);
		} else if constexpr (can_set_size<Target> and !reservable<Target>) {
			// TODO: Use an iterator that includes a count if we do not have a
			// sized source range or a random-access iterator for the target
			auto const target_position = containers::begin(target);
			auto const new_end = containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
			auto const source_size = [&] {
				if constexpr (sized_range<Source>) {
					return containers::size(source);
				} else {
					return ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
				}
			}();
			target.set_size(source_size);
		} else if constexpr (lazy_push_backable<Target>) {
			copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
				::containers::lazy_push_back(target, make);
			});
		} else {
			member_assign(target, OPERATORS_FORWARD(source));
		}
	} else {
		static_assert(supports_lazy_insert_after<Target>);
		copy_or_relocate_from(OPERATORS_FORWARD(source), [&target, it = target.before_begin()](auto make) mutable {
			it = target.lazy_insert_after(it, make);
		});
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
