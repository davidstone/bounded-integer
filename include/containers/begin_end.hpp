// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/contiguous_iterator.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {

template<typename Range>
concept has_member_begin = requires(Range range) {
	OPERATORS_FORWARD(range).begin();
};

template<typename Range>
concept beginable = std::is_array_v<std::remove_reference_t<Range>> or has_member_begin<Range>;

} // namespace detail

template<detail::beginable Range>
constexpr auto begin(Range && range) {
	if constexpr (detail::has_member_begin<Range>) {
		return OPERATORS_FORWARD(range).begin();
	} else {
		static_assert(std::is_array_v<std::remove_reference_t<Range>>);
		auto const base = contiguous_iterator(std::begin(range));
		if constexpr (std::is_rvalue_reference_v<Range>) {
			return move_iterator(base);
		} else {
			return base;
		}
	}
}

namespace detail {

template<typename Range>
concept has_member_end = requires(Range range) {
	OPERATORS_FORWARD(range).end();
};

template<typename Range>
concept has_member_size = requires(Range const & range) {
	range.size();
};

template<typename Range>
concept endable =
	std::is_array_v<std::remove_reference_t<Range>> or
	has_member_end<Range> or
	(has_member_begin<Range> and has_member_size<Range>);

} // namespace detail

template<detail::endable Range>
constexpr auto end(Range && range) {
	if constexpr (detail::has_member_end<Range>) {
		return OPERATORS_FORWARD(range).end();
	} else if constexpr (detail::has_member_begin<Range> and detail::has_member_size<Range>) {
		return OPERATORS_FORWARD(range).begin() + range.size();
	} else {
		static_assert(std::is_array_v<std::remove_reference_t<Range>>);
		auto const base = contiguous_iterator(std::end(range));
		if constexpr (std::is_rvalue_reference_v<Range>) {
			return move_iterator(base);
		} else {
			return base;
		}
	}
}

} // namespace containers
