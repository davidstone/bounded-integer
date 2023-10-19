// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.assign;

import containers.algorithms.advance;
import containers.algorithms.copy;
import containers.algorithms.erase;
import containers.algorithms.move_iterator;
import containers.append;
import containers.append_after;
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.clear;
import containers.dereference;
import containers.erase_concepts;
import containers.is_container;
import containers.iterator_t;
import containers.member_assign;
import containers.range;
import containers.range_value_t;
import containers.range_view;
import containers.resizable_container;
import containers.supports_lazy_insert_after;

import std_module;

namespace containers {

template<typename Target>
concept should_use_after_algorithms = supports_lazy_insert_after<Target> and has_member_erase_after<Target>;

template<typename Source, typename Target>
concept range_assignable_to =
	range<Source> and (
		member_assignable<Target, Source> or
		// TODO: Wrong type to assign from
		(
			std::is_assignable_v<range_value_t<Target> &, range_value_t<Source>> and
			(resizable_container<Target> or should_use_after_algorithms<Target>)
		)
	);

template<typename T>
concept has_capacity = requires(T const & container) {
	container.capacity();
};

template<typename Target, typename Source>
constexpr void assign_impl(Target & target, Source && source) {
	// Either of the two generic implementations (the last two options) work for
	// any type. For types that are not trivially copyable, we want to reuse the
	// existing storage where that makes sense. For types that are trivially
	// copyable, that concept does not make sense, so we instead use code that
	// optimizes better.
	if constexpr (member_assignable<Target, Source>) {
		::containers::member_assign(target, OPERATORS_FORWARD(source));
	} else if constexpr (should_use_after_algorithms<Target>) {
		auto const remainder = ::containers::copy_after(OPERATORS_FORWARD(source), target);
		::containers::erase_after(target, remainder.output, containers::end(target));
		::containers::append_after(target, remainder.output, range_view(remainder.input, containers::end(source)));
	} else if constexpr (std::is_trivially_copyable_v<range_value_t<Target>> and has_capacity<Target>) {
		::containers::clear(target);
		::containers::assign_to_empty(target, OPERATORS_FORWARD(source));
	} else {
		auto result = ::containers::copy(OPERATORS_FORWARD(source), target);
		::containers::erase_to_end(target, std::move(result.output));
		if constexpr (is_container<Source>) {
			::containers::append(
				target,
				range_view(
					move_iterator(std::move(result.input)),
					move_iterator(containers::end(OPERATORS_FORWARD(source)))
				)
			);
		} else {
			::containers::append(
				target,
				range_view(
					std::move(result.input),
					containers::end(OPERATORS_FORWARD(source))
				)
			);
		}
	}
}

// The behavior is undefined if `source` references elements in `target`
export template<range Target>
constexpr void assign(Target & target, range_assignable_to<Target> auto && source) {
	::containers::assign_impl(target, OPERATORS_FORWARD(source));
}
export constexpr void assign(range auto & target, empty_c_array_parameter) {
	::containers::clear(target);
}
export template<range Target, std::size_t init_size> requires range_assignable_to<c_array<range_value_t<Target>, init_size>, Target>
constexpr void assign(Target & target, c_array<range_value_t<Target>, init_size> && source) {
	::containers::assign_impl(target, std::move(source));
}

} // namespace containers
