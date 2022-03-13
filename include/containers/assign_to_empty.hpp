// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/appendable_from_capacity.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/initializer_range.hpp>
#include <containers/is_range.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/resizable_container.hpp>
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

template<typename Target>
constexpr auto get_source_size(auto const & source) {
	auto const value = containers::detail::linear_size(source);
	if constexpr (bounded::bounded_integer<decltype(value)>) {
		return value;
	} else {
		return ::bounded::assume_in_range<range_size_t<Target>>(value);
	}
};

template<typename Target, typename Source>
constexpr auto maybe_reserve(Target & target, Source const & source, auto reserve) {
	if constexpr (forward_range<Source> and reservable<Target>) {
		auto const source_size = ::containers::detail::get_source_size<Target>(source);
		reserve(target, source_size);
	}
}

template<typename Target, typename Source>
constexpr auto assign_to_empty_or_append(Target & target, Source && source, auto reserve, auto get_target_position, auto fallback) {
	if constexpr (appendable_from_capacity<Target> and reservable<Target> and size_then_use_range<Source>) {
		auto const source_size = ::containers::detail::get_source_size<Target>(source);
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
				return ::bounded::assume_in_range<range_size_t<Target>>(new_end - target_position);
			}
		}();
		target.append_from_capacity(source_size);
	} else if constexpr (lazy_push_backable<Target>) {
		::containers::detail::maybe_reserve(target, source, reserve);
		auto const last = containers::end(OPERATORS_FORWARD(source));
		for (auto it = containers::begin(OPERATORS_FORWARD(source)); it != last; ++it) {
			::containers::lazy_push_back(target, [&] { return *it; });
		}
	} else {
		fallback(target, OPERATORS_FORWARD(source));
	}
}

inline constexpr auto exact_reserve = []<typename Container>(Container & container, auto const requested_size) {
	container.reserve(::bounded::assume_in_range<range_size_t<Container>>(requested_size));
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

template<resizable_container Target, initializer_range<Target> Source>
constexpr auto assign_to_empty(Target & target, Source && source) -> void {
	::containers::detail::assign_to_empty_impl(target, OPERATORS_FORWARD(source));
}

constexpr auto assign_to_empty(resizable_container auto & target, empty_c_array_parameter) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
}
template<resizable_container Target, std::size_t init_size> requires std::is_move_constructible_v<range_value_t<Target>>
constexpr auto assign_to_empty(Target & target, c_array<range_value_t<Target>, init_size> && source) -> void {
	::containers::detail::assign_to_empty_impl(target, std::move(source));
}

} // namespace containers
