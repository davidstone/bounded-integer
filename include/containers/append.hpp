// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_container.hpp>
#include <containers/is_range.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>

#include <bounded/integer.hpp>

#include <operators/forward.hpp>

#include <utility>

namespace containers {
namespace detail {

inline constexpr auto exponential_reserve = [](auto & target, auto const source_size) {
	auto const current_size = ::containers::linear_size(target);
	if (current_size + source_size > target.capacity()) {
		target.reserve(::containers::detail::reallocation_size(target.capacity(), current_size, source_size));
	}
};

template<typename Target, typename Source>
constexpr auto maybe_reserve(Target & target, Source const & source) {
	if constexpr (size_then_use_range<Source> and reservable<Target>) {
		auto const source_size = ::containers::detail::get_source_size<Target>(source);
		exponential_reserve(target, source_size);
	}
}

template<typename Target, typename Source>
constexpr auto append_impl(Target & target, Source && source) -> void {
	if constexpr (can_set_size<Target> and reservable<Target> and size_then_use_range<Source>) {
		auto const source_size = ::containers::detail::get_source_size<Target>(source);
		exponential_reserve(target, source_size);
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), containers::end(target));
		target.set_size(containers::size(target) + source_size);
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
		copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
			::containers::lazy_push_back(target, make);
		});
	} else {
		using value_t = range_value_t<Target>;
		constexpr auto sufficiently_trivial = std::is_trivially_move_assignable_v<value_t>;
		decltype(auto) transformed = [&]() -> decltype(auto) {
			if constexpr (is_container<Source>) {
				return containers::transform(source, [](auto & value) -> auto && { return std::move(value); });
			} else {
				return OPERATORS_FORWARD(source);
			}
		}();
		constexpr auto has_member_insert = requires {
			target.insert(
				containers::end(target),
				make_legacy_iterator(containers::begin(transformed)),
				make_legacy_iterator(containers::end(transformed))
			);
		};
		if constexpr (sufficiently_trivial and has_member_insert) {
			target.insert(
				containers::end(target),
				make_legacy_iterator(containers::begin(transformed)),
				make_legacy_iterator(containers::end(transformed))
			);
		} else {
			::containers::detail::maybe_reserve(target, source);
			for (decltype(auto) value : OPERATORS_FORWARD(transformed)) {
				::containers::push_back(target, OPERATORS_FORWARD(value));
			}
		}
	}
}

} // namespace detail

// I would like to return an iterator to the start of the appended range, but
// that does not seem possible to do efficiently in general due to potential
// iterator instability.
//
// TODO: Support the source range being a subset of the target range

constexpr auto append(detail::push_backable auto & target, range auto && source) -> void {
	::containers::detail::append_impl(target, OPERATORS_FORWARD(source));
}

template<detail::push_backable Target>
constexpr auto append(Target &, empty_c_array_parameter) -> void {
}
template<detail::push_backable Target, std::size_t init_size>
constexpr auto append(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::detail::append_impl(target, std::move(source));
}

} // namespace containers
