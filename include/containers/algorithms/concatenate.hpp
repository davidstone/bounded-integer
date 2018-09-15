// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/append.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {
namespace detail {

// This assumes that the begining of the result is full of unused memory, and
// all elements in result are already where they belong.
template<typename Result, typename Range, typename... Ranges>
constexpr auto concatenate_prepend_append(Result & result, typename Result::iterator const it, Range && range, Ranges && ... ranges) {
	if constexpr (std::is_same_v<Result, std::remove_reference_t<Range>>) {
		if (std::addressof(result) == std::addressof(range)) {
			(..., append(result, BOUNDED_FORWARD(ranges)));
			return;
		}
	}
	auto const next_it = ::containers::uninitialized_copy(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), it);
	concatenate_prepend_append(result, next_it, BOUNDED_FORWARD(ranges)...);
}

template<typename Result>
constexpr auto concatenate_prepend_append(Result &, typename Result::iterator) {
	assert(false);
}

template<typename Result, typename Integer>
struct reusable_concatenate_t {
	Result * ptr;
	Integer before_size;
};

template<typename Result, typename Integer, typename Range>
constexpr auto reusable_concatenate_container(reusable_concatenate_t<Result, Integer> result, Integer const total_size, Range && range) {
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
		constexpr auto max =  std::min(std::numeric_limits<Size>::max(), std::numeric_limits<std::uintmax_t>::max() / 64);
		return static_cast<bounded::integer<0, max>>(size);
	} else {
		return size;
	}
}

}	// namespace detail

// TODO: support non-sized input ranges, splicable output ranges, and
// non-reservable output ranges
template<typename Result, typename... Ranges>
constexpr auto concatenate(Ranges && ... ranges) {
	auto const total_size = (0_bi + ... + ::containers::detail::ugly_size_hack(size(ranges)));
	using Integer = std::remove_const_t<decltype(total_size)>;

	auto reusable = detail::reusable_concatenate_t<Result, Integer>{nullptr, 0_bi};
	(..., (reusable = ::containers::detail::reusable_concatenate_container(reusable, total_size, BOUNDED_FORWARD(ranges))));
	if (reusable.ptr) {
		auto & ref = *reusable.ptr;
		auto const new_begin = data(ref) + reusable.before_size;
		::containers::uninitialized_move_destroy(containers::rbegin(ref), containers::rend(ref), containers::reverse_iterator(new_begin + size(ref)));
		ref.append_from_capacity(reusable.before_size);
		::containers::detail::concatenate_prepend_append(ref, begin(ref), BOUNDED_FORWARD(ranges)...);
		return std::move(ref);
	}

	Result result;
	result.reserve(static_cast<typename Result::size_type>(total_size));
	(..., append(result, BOUNDED_FORWARD(ranges)));
	return result;
}

}	// namespace containers
