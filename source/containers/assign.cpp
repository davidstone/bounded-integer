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
import containers.assign_to_empty;
import containers.begin_end;
import containers.c_array;
import containers.clear;
import containers.dereference;
import containers.is_container;
import containers.is_range;
import containers.iterator_t;
import containers.range_value_t;
import containers.range_view;
import containers.resizable_container;
import containers.splicable;
import containers.supports_lazy_insert_after;

import std_module;

namespace containers {

template<typename T>
concept supports_erase_after = requires(T & container, iterator_t<T const &> const it) {
	{ container.erase_after(it) } -> std::same_as<iterator_t<T &>>;
};

template<typename Target>
concept should_use_after_algorithms = supports_lazy_insert_after<Target> and supports_erase_after<Target>;

template<typename Source, typename Target>
concept range_assignable_to =
	range<Source> and (
		member_range_assignable<Target, Source> or
		member_iterator_assignable<Target, Source> or
		// TODO: Wrong type to assign from
		(
			std::is_assignable_v<range_value_t<Target> &, range_value_t<Source>> and
			(resizable_container<Target> or should_use_after_algorithms<Target>)
		)
	);

template<typename Target, typename Source>
constexpr void assign_impl(Target & target, Source && source) {
	// Either of the two generic implementations (the last two options) work for
	// any type. For types that are not trivially copyable, we want to reuse the
	// existing storage where that makes sense. For types that are trivially
	// copyable, that concept does not make sense, so we instead use code that
	// optimizes better.
	if constexpr (member_range_assignable<Target, Source> or member_iterator_assignable<Target, Source>) {
		member_assign(target, OPERATORS_FORWARD(source));
	} else if constexpr (should_use_after_algorithms<Target>) {
		auto target_before_it = target.before_begin();
		auto target_it = containers::next(target_before_it);
		auto const target_last = containers::end(target);
		auto source_it = containers::begin(source);
		auto const source_last = containers::end(source);

		// TODO: Add *_after algorithms to do this work
		// copy
		for (; source_it != source_last and target_it != target_last; ++source_it, ++target_it) {
			*target_it = dereference<Source>(source_it);
			target_before_it = target_it;
		}

		// erase remaining
		while (target_it != target_last) {
			target_it = target.erase_after(target_before_it);
		}

		// copy remaining
		for (; source_it != source_last; ++source_it) {
			target_before_it = target.lazy_insert_after(target_before_it, [&] -> decltype(auto) { return dereference<Source>(source_it); });
		}
	} else if constexpr (std::is_trivially_copyable_v<range_value_t<Target>>) {
		::containers::clear(target);
		::containers::assign_to_empty(target, OPERATORS_FORWARD(source));
	} else {
		auto result = ::containers::copy(OPERATORS_FORWARD(source), OPERATORS_FORWARD(target));
		::containers::erase_after(target, std::move(result.output));
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
