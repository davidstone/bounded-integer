// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/erase.hpp>
#include <containers/append.hpp>
#include <containers/assign_to_empty.hpp>
#include <containers/c_array.hpp>
#include <containers/clear.hpp>
#include <containers/is_range.hpp>
#include <containers/range_value_t.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Source, typename Target>
concept range_assignable_to =
	range<Source> and (
		member_range_assignable<Target, Source> or
		member_iterator_assignable<Target, Source> or
		// TODO: Wrong type to assign from
		(resizable_container<Target> and std::is_assignable_v<range_value_t<Target> &, range_value_t<Source>>)
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
	} else if constexpr (std::is_trivially_copyable_v<range_value_t<Target>>) {
		::containers::clear(target);
		::containers::assign_to_empty(target, OPERATORS_FORWARD(source));
	} else {
		auto result = ::containers::copy(OPERATORS_FORWARD(source), OPERATORS_FORWARD(target));
		::containers::erase_after(target, std::move(result.output));
		::containers::append(target, range_view(std::move(result.input), containers::end(OPERATORS_FORWARD(source))));
	}
}

} // namespace detail

// The behavior is undefined if `source` references elements in `target`
template<range Target>
constexpr void assign(Target & target, detail::range_assignable_to<Target> auto && source) {
	containers::detail::assign_impl(target, OPERATORS_FORWARD(source));
}
constexpr void assign(range auto & target, empty_c_array_parameter) {
	containers::clear(target);
}
template<range Target, std::size_t init_size> requires detail::range_assignable_to<c_array<range_value_t<Target>, init_size>, Target>
constexpr void assign(Target & target, c_array<range_value_t<Target>, init_size> && source) {
	containers::detail::assign_impl(target, std::move(source));
}

} // namespace containers
