// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/can_set_size.hpp>
#include <containers/data.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iterator_t.hpp>
#include <containers/resizable_container.hpp>
#include <containers/resize.hpp>
#include <containers/size.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/forward.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Result>
constexpr auto concatenate_prepend_append(Result &, iterator_t<Result &>) {
	std::unreachable();
}

// This assumes that the beginning of the result is full of unused memory, and
// all elements in result are already where they belong.
template<typename Result>
constexpr auto concatenate_prepend_append(Result & result, iterator_t<Result &> const it, auto && range, auto && ... ranges) {
	if constexpr (std::is_same_v<Result, std::remove_reference_t<decltype(range)>>) {
		if (std::addressof(result) == std::addressof(range)) {
			(..., ::containers::append(result, OPERATORS_FORWARD(ranges)));
			return;
		}
	}
	auto const next_it = ::containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(range), it);
	::containers::detail::concatenate_prepend_append(result, next_it, OPERATORS_FORWARD(ranges)...);
}

template<typename Result, typename Integer>
struct reusable_concatenate_t {
	Result * ptr;
	Integer before_size;
};

template<typename Result, typename Integer, typename Range>
constexpr auto reusable_concatenate_container(reusable_concatenate_t<Result, Integer> result, auto const total_size, Range && range) {
	if (result.ptr) {
		return result;
	}
	if constexpr (std::is_same_v<Result, Range>) {
		if (range.capacity() >= total_size) {
			return reusable_concatenate_t<Result, Integer>{std::addressof(range), result.before_size};
		}
	}
	result.before_size += size(range);
	return result;
}

// Adding up a bunch of sizes leads to overflow in bounds
template<typename Size>
constexpr auto ugly_size_hack(Size const size) {
	if constexpr (std::is_integral_v<Size>) {
		using result_t = bounded::integer<
			0,
			bounded::normalize<std::min(numeric_traits::max_value<Size>, numeric_traits::max_value<std::uintmax_t> / 64)>
		>;
		return ::bounded::assume_in_range<result_t>(size);
	} else {
		return size;
	}
}

template<typename Container>
concept can_reuse_storage =
	can_set_size<Container> or
	(std::is_trivial_v<range_value_t<Container>> and resizable_container<Container>);

template<typename Container>
constexpr auto move_existing_data_to_final_position(Container & container, auto const before_size) {
	if constexpr (can_set_size<Container>) {
		// Use data instead of begin to construct an iterator after `end()`.
		auto const new_begin = containers::data(container) + before_size;
		::containers::uninitialized_relocate(containers::reversed(container), containers::reverse_iterator(new_begin + ::containers::size(container)));
		container.set_size(containers::size(container) + before_size);
	} else {
		static_assert(std::is_trivial_v<range_value_t<Container>> and resizable_container<Container>);
		using difference_type = iter_difference_t<iterator_t<Container &>>;
		auto const original_size = bounded::integer(::containers::size(container));
		auto const new_size = original_size + before_size;
		::containers::resize(container, new_size);
		::containers::copy_backward(
			containers::range_view(containers::begin(container), containers::begin(container) + static_cast<difference_type>(original_size)),
			containers::begin(container) + static_cast<difference_type>(new_size)
		);
	}
}

}	// namespace detail

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
template<typename Result>
constexpr auto concatenate(auto && ... ranges) {
	auto const total_size = (0_bi + ... + ::containers::detail::ugly_size_hack(::containers::size(ranges)));

	if constexpr (detail::can_reuse_storage<Result>) {
		using Integer = bounded::integer<0, bounded::builtin_max_value<decltype(total_size)>>;
		auto reusable = detail::reusable_concatenate_t<Result, Integer>{nullptr, 0_bi};
		(..., (reusable = ::containers::detail::reusable_concatenate_container(reusable, total_size, OPERATORS_FORWARD(ranges))));
		if (reusable.ptr) {
			auto & ref = *reusable.ptr;
			if (reusable.before_size != 0_bi) {
				::containers::detail::move_existing_data_to_final_position(ref, reusable.before_size);
			}
			::containers::detail::concatenate_prepend_append(ref, containers::begin(ref), OPERATORS_FORWARD(ranges)...);
			return std::move(ref);
		}
	}

	auto result = Result();
	result.reserve(::bounded::assume_in_range<range_size_t<Result>>(total_size));
	(..., ::containers::append(result, OPERATORS_FORWARD(ranges)));
	return result;
}

}	// namespace containers
