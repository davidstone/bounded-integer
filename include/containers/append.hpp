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

inline constexpr auto append_fallback = []<typename Target, typename Source>(Target & target, Source && source) {
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
		::containers::detail::maybe_reserve(target, source, exponential_reserve);
		for (decltype(auto) value : OPERATORS_FORWARD(transformed)) {
			::containers::push_back(target, OPERATORS_FORWARD(value));
		}
	}
};

constexpr auto append_impl(auto & target, auto && source) -> void {
	::containers::detail::assign_to_empty_or_append(
		target,
		OPERATORS_FORWARD(source),
		exponential_reserve,
		[&] { return containers::end(target); },
		append_fallback
	);
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
