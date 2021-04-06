// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_range.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/resizable_container.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Range>
concept size_then_use_range = forward_range<Range const &> or requires(Range const & r) { containers::size(r); };

} // namespace detail

template<resizable_container Target, range Source>
constexpr auto assign_to_empty(Target & target, Source && source) -> void {
	BOUNDED_ASSERT(containers::is_empty(target));
	if constexpr (detail::appendable_from_capacity<Target> and detail::reservable<Target> and detail::size_then_use_range<Source>) {
		auto const source_size = static_cast<typename Target::size_type>(containers::detail::linear_size(source));
		target.reserve(source_size);
		containers::uninitialized_copy(OPERATORS_FORWARD(source), containers::begin(target));
		target.append_from_capacity(source_size);
	} else if constexpr (detail::appendable_from_capacity<Target> and !detail::reservable<Target>) {
		auto const it = containers::uninitialized_copy(OPERATORS_FORWARD(source), containers::begin(target));
		auto const source_size = [&] {
			if constexpr (sized_range<Source>) {
				return containers::size(source);
			} else {
				return static_cast<typename Target::size_type>(it - containers::begin(target));
			}
		}();
		target.append_from_capacity(source_size);
	} else if constexpr (detail::lazy_push_backable<Target>) {
		::containers::detail::lazy_push_back_range(target, OPERATORS_FORWARD(source));
	} else {
		::containers::detail::push_back_range(target, OPERATORS_FORWARD(source));
	}
}

template<resizable_container Container, std::size_t init_size>
constexpr void assign_to_empty(Container & destination, c_array<range_value_t<Container>, init_size> && init) {
	assign(destination, range_view(std::move(init)));
}

} // namespace containers
