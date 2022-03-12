// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/c_array.hpp>
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
concept has_member_before_begin = requires(Range range) {
	OPERATORS_FORWARD(range).before_begin();
};

template<typename Range>
concept should_use_before_begin = has_member_before_begin<Range> and !has_member_begin<Range>;

} // namespace detail

constexpr auto begin(detail::has_member_begin auto && range) {
	return OPERATORS_FORWARD(range).begin();
}

constexpr auto begin(detail::should_use_before_begin auto && range) {
	return ::containers::next(OPERATORS_FORWARD(range).before_begin());
}

template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> const & a) {
	return contiguous_iterator<T const, size>(a);
}

template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> & a) {
	return contiguous_iterator<T, size>(a);
}

template<typename T, std::size_t size>
constexpr auto begin(c_array<T, size> && a) {
	return contiguous_iterator<T, size>(a);
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
	has_member_end<Range> or
	(has_member_begin<Range> and has_member_size<Range>);

} // namespace detail

template<detail::endable Range>
constexpr auto end(Range && range) {
	if constexpr (detail::has_member_end<Range>) {
		return OPERATORS_FORWARD(range).end();
	} else {
		static_assert(detail::has_member_begin<Range> and detail::has_member_size<Range>);
		return OPERATORS_FORWARD(range).begin() + range.size();
	}
}

template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> const & a) {
	return contiguous_iterator<T const, size>(a + size);
}

template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> & a) {
	return contiguous_iterator<T, size>(a + size);
}

template<typename T, std::size_t size>
constexpr auto end(c_array<T, size> && a) {
	return contiguous_iterator<T, size>(a + size);
}

namespace detail {

using ::containers::begin;
using ::containers::end;

} // namespace detail
} // namespace containers
