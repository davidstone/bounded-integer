// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_range.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <operators/forward.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Target, typename Source>
concept member_iterator_assignable = requires(Target & target, Source && source) {
	target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
};

template<typename Target, typename Source>
concept member_range_assignable = requires(Target & target, Source && source) {
	target.assign(OPERATORS_FORWARD(source));
};

inline constexpr auto member_assign = []<typename Target, typename Source>(Target & target, Source && source) {
	if constexpr (member_range_assignable<Target, Source>) {
		target.assign(OPERATORS_FORWARD(source));
	} else {
		static_assert(member_iterator_assignable<Target, Source>);
		// TODO: Reserve for sized forward input ranges
		target.assign(containers::begin(OPERATORS_FORWARD(source)), containers::end(OPERATORS_FORWARD(source)));
	}
};


template<typename Target, typename Source>
constexpr auto assign_to_empty_or_append(Target & target, Source && source, auto reserve, auto get_target_position, auto fallback) {
	auto get_source_size = [&] {
		auto const value = containers::detail::linear_size(source);
		if constexpr (bounded::bounded_integer<decltype(value)>) {
			return value;
		} else {
			// TODO: This cast might be hiding an overflow bug
			return static_cast<range_size_t<Target>>(value);
		}
	};
	if constexpr (appendable_from_capacity<Target> and reservable<Target> and size_then_use_range<Source>) {
		auto const source_size = get_source_size();
		reserve(target, source_size);
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), get_target_position());
		target.append_from_capacity(source_size);
	} else if constexpr (appendable_from_capacity<Target> and !reservable<Target>) {
		auto const target_position = get_target_position();
		// TODO: Use an iterator that includes a count if we do not have a sized
		// source range or a random-access iterator for the target
		auto const new_end = containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(source), target_position);
		auto const source_size = [&] {
			if constexpr (sized_range<Source>) {
				return containers::size(source);
			} else {
				return static_cast<range_size_t<Target>>(new_end - target_position);
			}
		}();
		target.append_from_capacity(source_size);
	} else if constexpr (lazy_push_backable<Target>) {
		if constexpr (forward_range<Source> and reservable<Target>) {
			auto const source_size = get_source_size();
			reserve(target, source_size);
		}
		auto const last = containers::end(OPERATORS_FORWARD(source));
		for (auto it = containers::begin(OPERATORS_FORWARD(source)); it != last; ++it) {
			::containers::lazy_push_back(target, [&] { return *it; });
		}
	} else {
		fallback(target, OPERATORS_FORWARD(source));
	}
}

inline constexpr auto exact_reserve = []<typename Container>(Container & container, auto const requested_size) {
	// TODO: How to handle overflow here?
	container.reserve(static_cast<range_size_t<Container>>(requested_size));
};

constexpr auto assign_to_empty_impl(auto & target, auto && source) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
	::containers::detail::assign_to_empty_or_append(
		target,
		OPERATORS_FORWARD(source),
		exact_reserve,
		[&] { return containers::begin(target); },
		member_assign
	);
}

} // namespace detail

constexpr auto assign_to_empty(detail::push_backable auto & target, range auto && source) -> void {
	::containers::detail::assign_to_empty_impl(target, OPERATORS_FORWARD(source));
}

template<detail::push_backable Target, std::size_t init_size>
constexpr auto assign_to_empty(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::detail::assign_to_empty_impl(target, std::move(source));
}

} // namespace containers
