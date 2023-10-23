// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.legacy_append;

import containers.algorithms.copy_or_relocate_from;
import containers.algorithms.transform;
import containers.begin_end;
import containers.get_source_size;
import containers.is_container;
import containers.legacy_iterator;
import containers.linear_size;
import containers.range_value_t;
import containers.reallocation_size;
import containers.reservable;
import containers.size_then_use_range;

import std_module;

namespace containers {

template<typename Target, typename Source>
constexpr auto maybe_reserve(Target & target, Source const & source) {
	if constexpr (size_then_use_range<Source> and reservable<Target>) {
		auto const source_size = ::containers::get_source_size<Target>(source);
		auto const current_size = ::containers::linear_size(target);
		if (current_size + source_size > target.capacity()) {
			target.reserve(::containers::reallocation_size(target.capacity(), current_size, source_size));
		}
	}
}

export template<typename Target, typename Source>
constexpr auto legacy_append(Target & target, Source && source) -> void {
	constexpr auto sufficiently_trivial = std::is_trivially_move_assignable_v<range_value_t<Target>>;
	auto transformed = [&]() -> decltype(auto) {
		if constexpr (is_container<Source>) {
			return containers::transform(source, [](auto & value) -> auto && { return std::move(value); });
		} else {
			return OPERATORS_FORWARD(source);
		}
	};
	constexpr auto has_member_insert = requires(decltype(transformed()) t) {
		target.insert(
			containers::end(target),
			make_legacy_iterator(containers::begin(t)),
			make_legacy_iterator(containers::end(t))
		);
	};
	if constexpr (sufficiently_trivial and has_member_insert) {
		decltype(auto) t = transformed();
		target.insert(
			containers::end(target),
			make_legacy_iterator(containers::begin(t)),
			make_legacy_iterator(containers::end(t))
		);
	} else {
		// TODO: How do we handle aliasing issues?
		::containers::maybe_reserve(target, source);
		::containers::copy_or_relocate_from(OPERATORS_FORWARD(source), [&](auto make) {
			target.push_back(make());
		});
	}
}

} // namespace containers
