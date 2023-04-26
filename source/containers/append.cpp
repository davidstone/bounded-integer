// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.append;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.c_array;
import containers.can_set_size;
import containers.get_source_size;
import containers.is_range;
import containers.lazy_push_back;
import containers.legacy_append;
import containers.push_back;
import containers.range_value_t;
import containers.reallocation_size;
import containers.reserve_if_reservable;
import containers.size;
import containers.size_then_use_range;

import bounded;
import std_module;

namespace containers {
using namespace bounded::literal;

template<typename Target, typename Source>
constexpr auto append_impl(Target & target, Source && source) -> void {
	if constexpr (can_set_size<Target> and reservable<Target> and size_then_use_range<Source>) {
		auto const source_size = ::containers::get_source_size<Target>(source);
		auto const original_size = ::containers::size(target);
		auto const new_size = original_size + source_size;
		if (new_size <= target.capacity()) {
			containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), containers::end(target));
		} else {
			auto new_target = Target();
			new_target.reserve(::containers::reallocation_size(target.capacity(), original_size, source_size));
			containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), containers::begin(new_target) + original_size);
			containers::uninitialized_relocate_no_overlap(target, containers::begin(new_target));
			target.set_size(0_bi);
			target = std::move(new_target);
		}
		target.set_size(new_size);
	} else if constexpr (can_set_size<Target> and !reservable<Target>) {
		// TODO: Use an iterator that includes a count if we do not have a sized
		// source range or a random-access iterator for the target
		auto const target_position = containers::end(target);
		auto const new_end = containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
		auto const source_size = [&] {
			if constexpr (sized_range<Source>) {
				return containers::size(source);
			} else {
				return ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
			}
		}();
		target.set_size(containers::size(target) + source_size);
	} else if constexpr (lazy_push_backable<Target>) {
		// TODO: How do we handle aliasing issues here? Do we assume that all
		// containers that are not std-style containers and do not have
		// `set_size` are stable on `push_back`? Related to that, is `reserve`
		// ever helpful here? Does testing whether the target container is
		// reservable also test for stability?
		::containers::copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
			::containers::lazy_push_back(target, make);
		});
	} else {
		::containers::legacy_append(target, OPERATORS_FORWARD(source));
	}
}

// I would like to return an iterator to the start of the appended range, but
// that does not seem possible to do efficiently in general due to potential
// iterator instability.
export constexpr auto append(push_backable auto & target, range auto && source) -> void {
	::containers::append_impl(target, OPERATORS_FORWARD(source));
}

export template<push_backable Target>
constexpr auto append(Target &, empty_c_array_parameter) -> void {
}
export template<push_backable Target, std::size_t init_size>
constexpr auto append(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::append_impl(target, std::move(source));
}

} // namespace containers
